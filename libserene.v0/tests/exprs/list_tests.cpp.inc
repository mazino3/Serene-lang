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

#include "serene/errors.h"
#include "serene/exprs/expression.h"
#include "serene/exprs/list.h"
#include "serene/exprs/symbol.h"
#include "serene/namespace.h"
#include "serene/reader/reader.h"
#include "serene/semantics.h"

#include "../test_helpers.cpp.inc"

#include <llvm/Support/Error.h>

namespace serene {
namespace exprs {

TEST_CASE("List Expression", "[expression]") {
  std::unique_ptr<reader::LocationRange> range(dummyLocation());

  Node sym  = make<Symbol>(*range, "example", "user");
  Node sym1 = make<Symbol>(*range, "example1", "user");

  Node list = make<List>(*range);

  REQUIRE(list->getType() == ExprType::List);
  CHECK(list->toString() == "<List ->");

  Node list2 = make<List>(*range, list);
  CHECK(list2->toString() == "<List  <List ->>");

  Ast elements;
  elements.push_back(list);
  elements.push_back(list2);
  elements.push_back(sym);

  auto list3 = make<List>(*range, elements);

  CHECK(list3->toString() ==
        "<List  <List -> <List  <List ->> <Symbol user/example>>");

  auto l = llvm::dyn_cast<List>(list.get());

  l->append(sym1);

  REQUIRE(list->getType() == ExprType::List);
  CHECK(list->toString() == "<List  <Symbol user/example1>>");

  l->append(sym);
  REQUIRE(l->count() == 2);

  auto expr = l->at(1);
  REQUIRE(expr.hasValue());
  CHECK(expr.getValue()->toString() == "<Symbol user/example>");

  expr = l->at(2);
  REQUIRE_FALSE(expr.hasValue());

  for (auto x : *l) {
    CHECK(x->getType() == ExprType::Symbol);
  }
};

TEST_CASE("List semantic analysis of 'def'", "[semantic,expression,list]") {

  auto ctx = makeSereneContext();
  auto ns  = ctx->makeNamespace("user", llvm::None);
  auto ast = llvm::cantFail(READ("(def (a) b)"));

  SemanticEnv env;
  semantics::AnalysisState state(*ns, env);
  auto afterAst = semantics::analyze(state, ast);

  REQUIRE_FALSE(afterAst);
  {
    auto err = afterAst.takeError();
    CHECK(err.isA<llvm::ErrorList>());
  }
  ast      = llvm::cantFail(READ("(def a)"));
  afterAst = semantics::analyze(state, ast);
  REQUIRE_FALSE(afterAst);
  CHECK(errors::getMessage(afterAst.takeError()) == "Expected 3 got 2");

  ast      = llvm::cantFail(READ("(def a b c)"));
  afterAst = semantics::analyze(state, ast);
  REQUIRE_FALSE(afterAst);
  CHECK(errors::getMessage(afterAst.takeError()) == "Expected 3 got 4");

  ast      = llvm::cantFail(READ("(def a b)"));
  afterAst = semantics::analyze(state, ast);
  REQUIRE(afterAst);
  CHECK(astToString(&(*afterAst)) == "<Def a -> <Symbol b>>");

  ast      = llvm::cantFail(READ("(def a (fn () a))"));
  afterAst = semantics::analyze(state, ast);
  REQUIRE(afterAst);
  CHECK(astToString(&(*afterAst)) ==
        "<Def a -> <Fn a <List -> to <Symbol a>>>");
}

// TEST_CASE("List semantic analysis for 'fn'", "[semantic]") {
//   auto ctx      = makeSereneContext();
//   auto ns       = ctx->makeNamespace("user", llvm::None);
//   auto ast      = READ("(fn)");
//   auto afterAst = semantics::analyze(*ctx, *ast);

//   REQUIRE_FALSE(afterAst);
//   REQUIRE(afterAst.takeError().size() == 1);
//   CHECK(afterAst.getError()[0]->toString() ==
//         "<Error E3: The argument list is mandatory.>");

//   ast      = reader::read("(fn ())");
//   afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE(afterAst);
//   CHECK(astToString(&afterAst.getValue()) == "<Fn ___fn___0 <List -> to
//   <>>");

//   ast      = reader::read("(fn (a b c) a a a)");
//   afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE(afterAst);
//   CHECK(astToString(&afterAst.getValue()) ==
//         "<Fn ___fn___1 <List  <Symbol a> <Symbol b> <Symbol c>> to <Symbol a>
//         "
//         "<Symbol a> <Symbol a>>");

//   ast      = reader::read("(fn () a b)");
//   afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE(afterAst);
//   CHECK(astToString(&afterAst.getValue()) ==
//         "<Fn ___fn___2 <List -> to <Symbol a> <Symbol b>>");

//   ast      = reader::read("(fn (x) (fn (y) x) z)");
//   afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE(afterAst);
//   CHECK(astToString(&afterAst.getValue()) ==
//         "<Fn ___fn___4 <List  <Symbol x>> to <Fn ___fn___3 <List  <Symbol y>>
//         " "to <Symbol x>> <Symbol z>>");

//   ast      = reader::read("(fn (x) (def a b) (def b c))");
//   afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE(afterAst);
//   CHECK(astToString(&afterAst.getValue()) ==
//         "<Fn ___fn___5 <List  <Symbol x>> to <Def a -> <Symbol b>> <Def b ->
//         "
//         "<Symbol c>>>");
// }

// TEST_CASE("Complex semantic analysis", "[semantic]") {
//   auto ctx = makeSereneContext();
//   auto ns  = makeNamespace(*ctx, "user", llvm::None);
//   auto ast =
//       reader::read("(def a (fn (x) x))\n((def b (fn (x) (fn (y) y))))\n\n");
//   auto afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE(afterAst);
//   CHECK(astToString(&afterAst.getValue()) ==
//         "<Def a -> <Fn a <List  <Symbol x>> to <Symbol x>>> <Call <Def b -> "
//         "<Fn b <List  <Symbol x>> to <Fn ___fn___1 <List  <Symbol y>> to "
//         "<Symbol y>>>> >");

//   ctx      = makeSereneContext();
//   ns       = makeNamespace(*ctx, "user", llvm::None);
//   ast      = reader::read("((a b))");
//   afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE_FALSE(afterAst);
//   auto errs = afterAst.getError();
//   CHECK(errs[0]->toString() == "<Error E5: Can't resolve the symbol 'a'>");

//   ctx      = makeSereneContext();
//   ns       = makeNamespace(*ctx, "user", llvm::None);
//   ast      = reader::read("(def a (fn (x) x)) (a b)");
//   afterAst = semantics::analyze(*ctx, ast.getValue());
//   REQUIRE(afterAst);

//   CHECK(astToString(&afterAst.getValue()) ==
//         "<Def a -> <Fn a <List  <Symbol x>> to <Symbol x>>> <Call <Fn a <List
//         "
//         "<Symbol x>> to <Symbol x>> <Symbol b>>");
// }
} // namespace exprs
} // namespace serene
