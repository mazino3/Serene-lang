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

#ifndef COMPILER_H
#define COMPILER_H

#include "serene/llvm/IR/Value.h"
#include "serene/logger.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <string>

#if defined(ENABLE_LOG) || defined(ENABLE_COMPILER_LOG)
#define COMPILER_LOG(...) __LOG("COMPILER", __VA_ARGS__);
#else
#define COMPILER_LOG(...) ;
#endif

namespace serene {
// Forward declaration of State. The actual declaration is in state.hpp
class State;

class Compiler {

public:
  llvm::LLVMContext context;
  llvm::IRBuilder<> *builder;

  Compiler();

  State *state;
  llvm::Value *log_error(const char *s);
  void compile(std::string &input);

  ~Compiler();
};

} // namespace serene

#endif
