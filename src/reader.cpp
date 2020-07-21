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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string>
#include <iostream>
#include <memory>
#include <assert.h>
#include "serene/reader.hpp"
#include "serene/symbol.hpp"
#include "serene/list.hpp"

using namespace std;

namespace serene {
  Reader::Reader(const string &input) {
    input_stream.write(input.c_str(), input.size());
  };

  Reader::~Reader() {
    fmt::print("DELETE reader");
  }

  char Reader::get_char(const bool skip_whitespace) {
    for(;;) {
      char c = input_stream.get();
      if (skip_whitespace == true && isspace(c)) {
        continue;
      } else {
        return c;
      }
    }
  };

  void Reader::unget_char() {
    input_stream.unget();
  };

  bool Reader::is_valid_for_identifier(char c) {
    READER_LOG("IS: {}\n", c);
    switch(c) {
      case '!'
        | '$'
        | '%'
        | '&'
        | '*'
        | '+'
        | '-'
        | '.'
        | '~'
        | '/'
        | ':'
        | '<'
        | '='
        | '>'
        | '?'
        | '@'
        | '^'
        | '_':
        return true;
    }

    if((c >= 'a' && c <='z') ||
       (c >= 'A' && c <= 'Z') ||
       (c >= '0' && c <= '9')) {
      return true;
    }
    return false;
  }

  ast_node Reader::read_symbol() {
    bool empty = true;
    char c = get_char(false);

    READER_LOG("Read symbol\n");
    if(!this->is_valid_for_identifier(c)) {

      // TODO: Replece this with a tranceback function or something to raise
      // synatx error.
      fmt::print("Invalid character at the start of a symbol: '{}'\n", c);
      exit(1);
    }

    string sym("");

    while(c != EOF && ((!(isspace(c)) && this->is_valid_for_identifier(c)))) {
      sym += c;
      c = get_char(false);
      empty = false;
    }

    if (!empty) {
      unget_char();
      return make_unique<Symbol>(sym);
    }
    return nullptr;
  };

  ast_list_node Reader::read_list(List list) {
    char c = get_char(true);
    assert(c == '(');

    bool list_terminated = false;

    do {
      char c = get_char(true);

      switch(c) {
      case EOF:
        throw ReadError((char *)"EOF reached before closing of list");
      case ')':
        list_terminated = true;
        break;

      default:
        unget_char();
        auto tmp{read_expr()};
        list.add_tail(move(tmp));
      }

    } while(!list_terminated);

    return unique_ptr<List>(&list);
  }


  ast_node Reader::read_expr() {
    char c = get_char(false);
    READER_LOG("CHAR: {}\n", c);

    unget_char();

    switch(c) {
    case '(':
      return read_list(List());

    case EOF:
      return nullptr;

    default:
      return read_symbol();
    }
  }

  ast_tree &Reader::read() {
    char c = get_char(true);

    while(c != EOF) {
      unget_char();
      auto tmp{read_expr()};
      fmt::print("##### {}", tmp->string_repr());
      if(tmp) {
        this->ast.push_back(move(tmp));
      }
      c = get_char(true);
      READER_LOG("11111 {}\n", c);
    }

    READER_LOG("333333333333333\n");
    return this->ast;
  };
}
