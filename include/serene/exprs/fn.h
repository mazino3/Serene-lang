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

#ifndef EXPRS_FN_H
#define EXPRS_FN_H

#include "serene/errors/error.h"
#include "serene/exprs/expression.h"
#include "serene/exprs/list.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <memory>
#include <string>

namespace serene {

namespace exprs {
class List;

/// This data structure represents a function. with a collection of
/// arguments and the ast of a body
class Fn : public Expression {

public:
  std::string name;

  // TODO: Use a coll type instead of a list here
  List args;
  ast body;

  Fn(reader::LocationRange &loc, List &args, ast body)
      : Expression(loc), args(args), body(body){};

  ExprType getType() const;
  std::string toString() const;
  maybe_node analyze(reader::SemanticContext &);

  static bool classof(const Expression *e);

  /// Creates a function node out of a function definition
  /// in a list. the list has to contain the correct definition
  /// of a function, for exmaple: `(fn (args1 arg2) body)`
  static maybe_node make(List *);
  ~Fn() = default;
};

} // namespace exprs
} // namespace serene

#endif
