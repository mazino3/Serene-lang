#include "catch2/catch.hpp"
#include "serene/exprs/expression.h"
#include "serene/exprs/list.h"
#include "serene/exprs/symbol.h"
#include <iostream>

namespace serene {
namespace exprs {

reader::LocationRange *dummyLocation() {
  reader::Location start;
  reader::Location end;

  start.line = 2;
  start.col = 20;
  start.pos = 40;

  end.line = 3;
  end.col = 30;
  end.pos = 80;

  return new reader::LocationRange(start, end);
}

TEST_CASE("List Expression", "[expression]") {
  // return new reader::LocationRange(start, end);
  std::unique_ptr<reader::LocationRange> range(dummyLocation());
  Expression list = Expression::make<List>(*range.get());

  REQUIRE(list.toString() == "<List [loc: 2:20:40 | 3:30:80]: ->");
  REQUIRE(list.getType() == ExprType::List);
};
} // namespace exprs
} // namespace serene
