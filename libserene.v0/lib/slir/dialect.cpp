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

#include "serene/slir/dialect.h"

#include "serene/slir/dialect.cpp.inc"
#include "serene/slir/ops.h"
#include "serene/slir/types.h"

#include <mlir/IR/Builders.h>
#include <mlir/IR/Dialect.h>
#include <mlir/IR/DialectImplementation.h>
#include <mlir/IR/MLIRContext.h>

namespace serene {
namespace slir {

/// Dialect initialization, the instance will be owned by the context. This is
/// the point of registration of types and operations for the dialect.
void SereneDialect::initialize() {
  registerType();
  addOperations<
#define GET_OP_LIST
#include "serene/slir/ops.cpp.inc"
      >();
}

void registerTo(mlir::DialectRegistry &registry) {
  registry.insert<serene::slir::SereneDialect>();
};

} // namespace slir
} // namespace serene
