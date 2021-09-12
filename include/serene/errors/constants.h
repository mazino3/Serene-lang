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

#ifndef SERENE_ERRORS_CONSTANTS_H
#define SERENE_ERRORS_CONSTANTS_H

#include <map>
#include <string>

namespace serene {
namespace errors {

enum ErrID {
  E0000,
  E0001,
  E0002,
  E0003,
  E0004,
  E0005,
  E0006,
  E0007,
  E0008,
  E0009,
  E0010,
  E0011,
  E0012,
  E0013,
};

struct ErrorVariant {
  ErrID id;

  std::string description;
  std::string longDescription;

  ErrorVariant(ErrID id, std::string desc, std::string longDesc)
      : id(id), description(desc), longDescription(longDesc){};
};

static ErrorVariant
    UnknownError(E0000, "Can't find any description for this error.", "");
static ErrorVariant
    DefExpectSymbol(E0001, "The first argument to 'def' has to be a Symbol.",
                    "");

static ErrorVariant DefWrongNumberOfArgs(
    E0002, "Wrong number of arguments is passed to the 'def' form.", "");

static ErrorVariant FnNoArgsList(E0003, "'fn' form requires an argument list.",
                                 "");

static ErrorVariant FnArgsMustBeList(E0004, "'fn' arguments should be a list.",
                                     "");

static ErrorVariant CantResolveSymbol(E0005, "Can't resolve the given name.",
                                      "");
static ErrorVariant
    DontKnowHowToCallNode(E0006, "Don't know how to call the given expression.",
                          "");

static ErrorVariant PassFailureError(E0007, "Pass Failure.", "");
static ErrorVariant NSLoadError(E0008, "Faild to find a namespace.", "");
static ErrorVariant
    NSAddToSMError(E0009, "Faild to add the namespace to the source manager.",
                   "");

static ErrorVariant
    EOFWhileScaningAList(E0010, "EOF reached before closing of list", "");

static ErrorVariant InvalidDigitForNumber(E0011, "Invalid digit for a number.",
                                          "");

static ErrorVariant
    TwoFloatPoints(E0012, "Two or more float point characters in a number", "");
static ErrorVariant InvalidCharacterForSymbol(
    E0013, "Two or more float point characters in a number", "");

static std::map<ErrID, ErrorVariant *> ErrDesc = {
    {E0000, &UnknownError},          {E0001, &DefExpectSymbol},
    {E0002, &DefWrongNumberOfArgs},  {E0003, &FnNoArgsList},
    {E0004, &FnArgsMustBeList},      {E0005, &CantResolveSymbol},
    {E0006, &DontKnowHowToCallNode}, {E0007, &PassFailureError},
    {E0008, &NSLoadError},           {E0009, &NSAddToSMError},
    {E0010, &EOFWhileScaningAList},  {E0011, &InvalidDigitForNumber},
    {E0012, &TwoFloatPoints},        {E0013, &InvalidCharacterForSymbol}};

} // namespace errors
} // namespace serene
#endif
