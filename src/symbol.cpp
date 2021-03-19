/**
 * Serene programming language.
 *
 *  Copyright (c) 2020 Sameer Rahmani <lxsameer@gnu.org>
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

#include "serene/symbol.hpp"
#include "serene/compiler.hpp"
#include "serene/expr.hpp"
#include "serene/llvm/IR/Value.h"
#include "serene/namespace.hpp"
#include "serene/state.hpp"
#include <assert.h>
#include <fmt/core.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <string>

using namespace std;
using namespace llvm;

namespace serene {

string Symbol::string_repr() const { return name_; }

const string &Symbol::name() const { return name_; }

Symbol::Symbol(const string &name) : name_(name) {}

Value *Symbol::codegen(Compiler &compiler, State &state) {
  if (name() == "false") {
    return ConstantInt::get(Type::getInt1Ty(compiler.context), 0);
  }

  if (name() == "true") {
    return ConstantInt::get(Type::getInt1Ty(compiler.context), 1);
  }

  Value *V = state.lookup_in_current_scope(name());

  if (!V) {
    return compiler.log_error(
        fmt::format("Unable to resolve symbol '{}'.", name()).c_str());
  }
  return V;
}

Symbol::~Symbol() { EXPR_LOG("Destroying symbol"); }
} // namespace serene