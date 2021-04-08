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

#ifndef EXPRS_SYMBOL_H
#define EXPRS_SYMBOL_H

#include "serene/exprs/expression.h"
#include "llvm/ADT/SmallVector.h"
#include <string>

namespace serene {

namespace exprs {

class List {
public:
  reader::LocationRange location;
  llvm::SmallVector<Expression, 0> elements;

  List(const reader::LocationRange &loc) : location(loc), elements({}){};

  List(const reader::LocationRange &loc, llvm::ArrayRef<Expression> elems)
      : location(loc), elements(elems.begin(), elems.end()){};

  ExprType getType() { return ExprType::List; };
  std::string toString();

  ~List() = default;

  static List build(const reader::LocationRange &loc) { return List(loc); }
};

} // namespace exprs
} // namespace serene

#endif
