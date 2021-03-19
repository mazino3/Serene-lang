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

#ifndef NAMESPACE_H
#define NAMESPACE_H

#include "serene/llvm/IR/Value.h"
#include "serene/logger.hpp"
#include <llvm/IR/Module.h>
#include <string>

#if defined(ENABLE_LOG) || defined(ENABLE_NAMESPACE_LOG)
#define NAMESPACE_LOG(...) __LOG("NAMESPACE", __VA_ARGS__);
#else
#define NAMESPACE_LOG(...) ;
#endif

namespace serene {
class AExpr;
class List;
class Compiler;
class State;

class Namespace {
  // Why not ast_node ? because i have to include expr.hpp which
  // causes a circular dependency
  using MakerFn =
      std::function<std::shared_ptr<AExpr>(Compiler &, State &, const List *)>;
  using BuiltinMap = std::map<std::string, MakerFn>;
  std::unique_ptr<llvm::Module> module;
  std::map<std::string, llvm::Value *> scope;
  static BuiltinMap builtins;

public:
  std::string name;

  Namespace(std::string &n) : name(n){};
  llvm::Value *lookup(const std::string &name);
  void insert_symbol(const std::string &name, llvm::Value *v);

  void print_scope();
  ~Namespace();
};

} // namespace serene

#endif