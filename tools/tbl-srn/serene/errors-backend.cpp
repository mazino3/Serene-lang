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

// The "serene/" part is due to a convention that we use in the project
#include "serene/errors-backend.h"

#include <llvm/Support/Casting.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/LineIterator.h>
#include <llvm/Support/MemoryBufferRef.h>
#include <llvm/TableGen/Error.h>
#include <llvm/TableGen/Record.h>

#define DEBUG_TYPE      "errors-backend"
#define INSTANCE_SUFFIX "Instance"

namespace serene {

// Any helper data structures can be defined here. Some backends use
// structs to collect information from the records.

class ErrorsBackend {
private:
  llvm::RecordKeeper &records;

public:
  ErrorsBackend(llvm::RecordKeeper &rk) : records(rk) {}

  void createNSBody(llvm::raw_ostream &os);
  void createErrorClass(int id, llvm::Record &defRec, llvm::raw_ostream &os);
  void run(llvm::raw_ostream &os);
}; // emitter class

static void inNamespace(llvm::StringRef name, llvm::raw_ostream &os,
                        std::function<void(llvm::raw_ostream &)> f) {

  os << "namespace " << name << " {\n\n";
  f(os);
  os << "}; // namespace " << name << "\n";
};

void ErrorsBackend::createErrorClass(const int id, llvm::Record &defRec,
                                     llvm::raw_ostream &os) {
  (void)records;

  const auto recName = defRec.getName();

  os << "class " << recName << " : public llvm::ErrorInfo<" << recName << ", "
     << "SereneError> {\n"
     << "public:\n"
     << "  using llvm::ErrorInfo<" << recName << ", "
     << "SereneError>::ErrorInfo;\n"
     << "  constexpr static const int ID = " << id << ";\n};\n\n"
     << "static const ErrorVariant " << recName << INSTANCE_SUFFIX
     << " = ErrorVariant::make(\n"
     << "  " << id << ",\n"
     << "  \"" << recName << "\",\n";

  auto desc = defRec.getValueAsString("desc");

  if (desc.empty()) {
    llvm::PrintError("'desc' field is empty for " + recName);
  }

  os << "  \"" << desc << "\",\n";

  auto help = defRec.getValueAsString("help");

  if (!help.empty()) {

    const llvm::MemoryBufferRef value(help, "help");

    llvm::line_iterator lines(value, false);
    while (!lines.is_at_end()) {
      if (lines.line_number() != 1) {
        os << '\t';
      }
      auto prevLine = *lines;
      lines++;
      os << '"' << prevLine << '"';

      if (lines.is_at_end()) {
        os << ";\n";
      } else {
        os << '\n';
      }
    }
  } else {
    os << "  \"\"";
  }

  os << ");\n";
  // os << "  " << help << ");\n";
  //  auto *stringVal = llvm::dyn_cast<llvm::StringInit>(val.getValue());
};

void ErrorsBackend::createNSBody(llvm::raw_ostream &os) {
  auto *index = records.getGlobal("errorsIndex");

  if (index == nullptr) {
    llvm::PrintError("'errorsIndex' var is missing!");
    return;
  }

  auto *indexList = llvm::dyn_cast<llvm::ListInit>(index);

  if (indexList == nullptr) {
    llvm::PrintError("'errorsIndex' has to be a list!");
    return;
  }

  os << "#ifdef GET_CLASS_DEFS\n";
  inNamespace("serene::errors", os, [&](llvm::raw_ostream &os) {
    for (size_t i = 0; i < indexList->size(); i++) {

      // llvm::Record &defRec = *defPair.second;
      llvm::Record *defRec = indexList->getElementAsRecord(i);

      if (!defRec->isSubClassOf("Error")) {
        continue;
      }

      createErrorClass(i, *defRec, os);
    }
  });

  os << "#undef GET_CLASS_DEFS\n#endif\n\n";

  os << "#ifdef GET_ERRS_ARRAY\n\n";
  inNamespace("serene::errors", os, [&](llvm::raw_ostream &os) {
    os << "static const std::array<int, ErrorVariant *> "
          "variants{\n";
    for (size_t i = 0; i < indexList->size(); i++) {

      // llvm::Record &defRec = *defPair.second;
      llvm::Record *defRec = indexList->getElementAsRecord(i);

      if (!defRec->isSubClassOf("Error")) {
        continue;
      }
      os << "  &" << defRec->getName() << INSTANCE_SUFFIX << ",\n";
    }
  });
  os << "\n};\n#undef GET_ERRS_ARRAY\n#endif\n";
}

void ErrorsBackend::run(llvm::raw_ostream &os) {
  (void)records;
  llvm::emitSourceFileHeader("Serene's Errors collection", os);

  os << "#include \"serene/errors/base.h\"\n\n#include "
        "<llvm/Support/Error.h>\n\n";
  os << "#ifndef SERENE_ERRORS_ERRORS_H\n#define SERENE_ERRORS_ERRORS_H\n\n";
  createNSBody(os);
  os << "#endif\n";
}

void emitErrors(llvm::RecordKeeper &rk, llvm::raw_ostream &os) {
  ErrorsBackend(rk).run(os);
}

} // namespace serene
