/* -*- C++ -*-
 * Serene Programming Language
 *
 * Copyright (c) 2019-2022 Sameer Rahmani <lxsameer@gnu.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "serene/namespace.h"

#include "serene/context.h"
#include "serene/export.h"
#include "serene/exprs/expression.h"
#include "serene/llvm/IR/Value.h"
#include "serene/semantics.h"
#include "serene/slir/slir.h"

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/FormatVariadic.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Support/LogicalResult.h>

#include <memory>
#include <stdexcept>
#include <string>

using namespace std;
using namespace llvm;

namespace serene {

Namespace::Namespace(SereneContext &ctx, llvm::StringRef ns_name,
                     llvm::Optional<llvm::StringRef> filename)
    : ctx(ctx), name(ns_name) {
  if (filename.hasValue()) {
    this->filename.emplace(filename.getValue().str());
  }

  // Create the root environment
  createEnv(nullptr);
};

SemanticEnv &Namespace::createEnv(SemanticEnv *parent) {
  auto env = std::make_unique<SemanticEnv>(parent);
  environments.push_back(std::move(env));

  return *environments.back();
};

SemanticEnv &Namespace::getRootEnv() {
  assert(!environments.empty() && "Root env is not created!");

  return *environments.front();
};

mlir::LogicalResult Namespace::define(std::string &name, exprs::Node &node) {
  auto &rootEnv = getRootEnv();

  if (failed(rootEnv.insert_symbol(name, node))) {
    return mlir::failure();
  }

  symbolList.push_back(name);
  return mlir::success();
}

exprs::Ast &Namespace::getTree() { return this->tree; }

llvm::Error Namespace::addTree(exprs::Ast &ast) {

  // If the target phase is just parsing we don't want
  // to run the semantic analyzer or anything beyond parser
  if (ctx.getTargetPhase() == CompilationPhase::Parse) {
    // we just want the raw AST
    this->tree.insert(this->tree.end(), ast.begin(), ast.end());
    return llvm::Error::success();
  }

  auto &rootEnv = getRootEnv();

  auto state = semantics::makeAnalysisState(*this, rootEnv);
  // Run the semantic analyer on the ast and then if everything
  // is ok add the form to the tree and forms
  auto maybeForm = semantics::analyze(*state, ast);

  if (!maybeForm) {
    return maybeForm.takeError();
  }

  auto semanticAst = std::move(*maybeForm);
  this->tree.insert(this->tree.end(), semanticAst.begin(), semanticAst.end());

  return llvm::Error::success();
}

uint Namespace::nextFnCounter() { return fn_counter++; };

SereneContext &Namespace::getContext() { return this->ctx; };

MaybeModuleOp Namespace::generate(unsigned offset) {
  // The reason why we return an optional value instead of Errors
  // is the way MLIR's diagnostic engine works. Passes may use
  // the `emit` function of operations to report errors to the
  // diagnostic engine. So we can't return any error diractly.

  mlir::OpBuilder builder(&ctx.mlirContext);

  // TODO: Fix the unknown location by pointing to the `ns` form
  auto module = mlir::ModuleOp::create(builder.getUnknownLoc(),
                                       llvm::Optional<llvm::StringRef>(name));

  auto treeSize = getTree().size();

  // Walk the AST and call the `generateIR` function of each node.
  // Since nodes will have access to the a reference of the
  // namespace they can use the builder and keep adding more
  // operations to the module via the builder
  for (unsigned i = offset; i < treeSize; ++i) {
    auto &node = getTree()[i];
    node->generateIR(*this, module);
  }

  if (mlir::failed(mlir::verify(module))) {
    module.emitError("Can't verify the module");
    module.erase();
    return llvm::None;
  }

  if (mlir::failed(runPasses(module))) {
    // TODO: Report a proper error
    module.emitError("Failure in passes!");
    module.erase();
    return llvm::None;
  }

  return MaybeModuleOp(module);
}

mlir::LogicalResult Namespace::runPasses(mlir::ModuleOp &m) {
  return ctx.pm.run(m);
};

void Namespace::dump() {
  llvm::outs() << "\nMLIR: \n";
  auto maybeModuleOp = generate();

  if (!maybeModuleOp) {

    llvm::errs() << "Failed to generate the IR.\n";
    return;
  }

  mlir::OpPrintingFlags flags;
  flags.enableDebugInfo();

  maybeModuleOp.getValue()->print(llvm::outs(), flags);
};

MaybeModule Namespace::compileToLLVM() {
  // The reason why we return an optional value instead of Errors
  // is the way MLIR's diagnostic engine works. Passes may use
  // the `emit` function of operations to report errors to the
  // diagnostic engine. So we can't return any error diractly.

  auto maybeModule = generate();

  if (!maybeModule) {
    NAMESPACE_LOG("IR generation failed for '" << name << "'");
    return llvm::None;
  }

  if (ctx.getTargetPhase() >= CompilationPhase::IR) {
    mlir::ModuleOp module = maybeModule.getValue().get();
    return ::serene::slir::compileToLLVMIR(ctx, module);
  }

  return llvm::None;
};

MaybeModule Namespace::compileToLLVMFromOffset(unsigned offset) {
  // The reason why we return an optional value instead of Errors
  // is the way MLIR's diagnostic engine works. Passes may use
  // the `emit` function of operations to report errors to the
  // diagnostic engine. So we can't return any error diractly.

  auto maybeModule = generate(offset);

  if (!maybeModule) {
    NAMESPACE_LOG("IR generation failed for '" << name << "'");
    return llvm::None;
  }

  if (ctx.getTargetPhase() >= CompilationPhase::IR) {
    mlir::ModuleOp module = maybeModule.getValue().get();
    return ::serene::slir::compileToLLVMIR(ctx, module);
  }

  return llvm::None;
};

NSPtr Namespace::make(SereneContext &ctx, llvm::StringRef name,
                      llvm::Optional<llvm::StringRef> filename) {
  return std::make_shared<Namespace>(ctx, name, filename);
};

Namespace::~Namespace() {
  // TODO: Clean up anything related to this namespace in the context
  // TODO: Remove anything related to this namespace in the JIT
  NAMESPACE_LOG("Destructing NS: " << name);
};

} // namespace serene
