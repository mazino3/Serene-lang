/* -*- C++ -*-
 * Serene programming language.
 *
 *  Copyright (c) 2019-2021 Sameer Rahmani <lxsameer@gnu.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "serene/passes.h"
#include "serene/slir/dialect.h"

#include <llvm/Support/Casting.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/MemRef/IR/MemRef.h>
#include <mlir/Dialect/StandardOps/IR/Ops.h>
#include <mlir/IR/Attributes.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/Pass/Pass.h>
#include <mlir/Transforms/DialectConversion.h>

namespace serene::passes {

struct ValueOpLowering : public mlir::OpRewritePattern<serene::slir::ValueOp> {
  using OpRewritePattern<serene::slir::ValueOp>::OpRewritePattern;

  mlir::LogicalResult
  matchAndRewrite(serene::slir::ValueOp op,
                  mlir::PatternRewriter &rewriter) const final;
};

mlir::LogicalResult
ValueOpLowering::matchAndRewrite(serene::slir::ValueOp op,
                                 mlir::PatternRewriter &rewriter) const {
  auto value         = op.value();
  mlir::Location loc = op.getLoc();

  llvm::SmallVector<mlir::Type, 4> arg_types(0);
  auto func_type = rewriter.getFunctionType(arg_types, rewriter.getI64Type());
  auto fn        = rewriter.create<mlir::FuncOp>(loc, "randomname", func_type);
  if (!fn) {
    op.emitOpError("Value Rewrite fn is null");
    return mlir::failure();
  }

  auto entryBlock = fn.addEntryBlock();
  rewriter.setInsertionPointToStart(entryBlock);
  auto retVal = rewriter
                    .create<mlir::ConstantIntOp>(loc, (int64_t)value,
                                                 rewriter.getI64Type())
                    .getResult();

  mlir::ReturnOp returnOp = rewriter.create<mlir::ReturnOp>(loc, retVal);

  if (!returnOp) {
    op.emitError("Value Rewrite returnOp is null");
    return mlir::failure();
  }

  fn.setPrivate();
  rewriter.eraseOp(op);
  return mlir::success();
}

// Fn lowering pattern
struct FnOpLowering : public mlir::OpRewritePattern<serene::slir::FnOp> {
  using OpRewritePattern<serene::slir::FnOp>::OpRewritePattern;

  mlir::LogicalResult
  matchAndRewrite(serene::slir::FnOp op,
                  mlir::PatternRewriter &rewriter) const final;
};

mlir::LogicalResult
FnOpLowering::matchAndRewrite(serene::slir::FnOp op,
                              mlir::PatternRewriter &rewriter) const {
  auto args          = op.args();
  auto name          = op.name();
  auto isPublic      = op.sym_visibility().getValueOr("public") == "public";
  mlir::Location loc = op.getLoc();

  llvm::SmallVector<mlir::Type, 4> arg_types;

  for (auto &arg : args) {
    auto attr = std::get<1>(arg).dyn_cast<mlir::TypeAttr>();

    if (!attr) {
      op.emitError("It's not a type attr");
      return mlir::failure();
    }
    arg_types.push_back(attr.getValue());
  }

  auto func_type = rewriter.getFunctionType(arg_types, rewriter.getI64Type());
  auto fn        = rewriter.create<mlir::FuncOp>(loc, name, func_type);

  if (!fn) {
    op.emitError("Value Rewrite fn is null");
    return mlir::failure();
  }

  auto *entryBlock = fn.addEntryBlock();

  rewriter.setInsertionPointToStart(entryBlock);
  auto retVal =
      rewriter
          .create<mlir::ConstantIntOp>(loc, (int64_t)3, rewriter.getI64Type())
          .getResult();

  rewriter.create<mlir::ReturnOp>(loc, retVal);

  if (!isPublic) {
    fn.setPrivate();
  }

  rewriter.eraseOp(op);
  return mlir::success();
}

// SLIR lowering pass
struct SLIRToMLIRPass
    : public mlir::PassWrapper<SLIRToMLIRPass,
                               mlir::OperationPass<mlir::ModuleOp>> {
  void getDependentDialects(mlir::DialectRegistry &registry) const override;
  void runOnOperation() final;
  void runOnModule();
  mlir::ModuleOp getModule();
};

void SLIRToMLIRPass::getDependentDialects(
    mlir::DialectRegistry &registry) const {
  registry.insert<mlir::StandardOpsDialect>();
};

/// Return the current function being transformed.
mlir::ModuleOp SLIRToMLIRPass::getModule() { return this->getOperation(); }

void SLIRToMLIRPass::runOnOperation() { runOnModule(); }

void SLIRToMLIRPass::runOnModule() {

  auto module = getModule();

  // The first thing to define is the conversion target. This will define the
  // final target for this lowering.
  mlir::ConversionTarget target(getContext());

  // We define the specific operations, or dialects, that are legal targets for
  // this lowering. In our case, we are lowering to a combination of the
  // `Affine`, `MemRef` and `Standard` dialects.
  target.addLegalDialect<mlir::StandardOpsDialect>();

  // We also define the Toy dialect as Illegal so that the conversion will fail
  // if any of these operations are *not* converted. Given that we actually want
  // a partial lowering, we explicitly mark the Toy operations that don't want
  // to lower, `toy.print`, as `legal`.
  target.addIllegalDialect<serene::slir::SereneDialect>();
  // target.addLegalOp<serene::slir::PrintOp>();
  target.addLegalOp<mlir::FuncOp>();

  // Now that the conversion target has been defined, we just need to provide
  // the set of patterns that will lower the Toy operations.
  mlir::RewritePatternSet patterns(&getContext());
  patterns.add<ValueOpLowering, FnOpLowering>(&getContext());

  // With the target and rewrite patterns defined, we can now attempt the
  // conversion. The conversion will signal failure if any of our `illegal`
  // operations were not converted successfully.
  if (failed(applyPartialConversion(module, target, std::move(patterns)))) {
    signalPassFailure();
  }
};

std::unique_ptr<mlir::Pass> createSLIRLowerToMLIRPass() {
  return std::make_unique<SLIRToMLIRPass>();
};
} // namespace serene::passes