/*
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

#include "serene/exprs/list.h"
#include "llvm/Support/FormatVariadic.h"

namespace serene {
namespace exprs {

List::List(const List &l) : Expression(l.location){};
List::List(const reader::LocationRange &loc, node e) : Expression(loc) {
  elements.push_back(std::move(e));
};

List::List(const reader::LocationRange &loc, llvm::ArrayRef<node> elems)
    : Expression(loc), elements(elems.begin(), elems.end()){};

ExprType List::getType() const { return ExprType::List; };
std::string List::toString() const {
  std::string s{this->elements.empty() ? "-" : ""};

  for (auto &n : this->elements) {
    s = llvm::formatv("{0} {1}", s, n->toString());
  }

  return llvm::formatv("<List [loc: {0} | {1}]: {2}>",
                       this->location.start.toString(),
                       this->location.end.toString(), s);
};

maybe_node List::analyze(reader::SemanticContext &ctx) {
  return Result<node>::Success(node(this));
};

bool List::classof(const Expression *e) {
  return e->getType() == ExprType::List;
};

void List::append(node n) { elements.push_back(n); }
} // namespace exprs
} // namespace serene