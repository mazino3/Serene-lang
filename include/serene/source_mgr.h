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

#ifndef SERENE_SOURCE_MGR_H
#define SERENE_SOURCE_MGR_H

#include "serene/namespace.h"
#include "serene/reader/location.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/Support/ErrorHandling.h"

#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <memory>
#include <mlir/IR/Diagnostics.h>
#include <mlir/Support/Timing.h>

#define SMGR_LOG(...)                       \
  DEBUG_WITH_TYPE("sourcemgr", llvm::dbgs() \
                                   << "[SMGR]: " << __VA_ARGS__ << "\n");

namespace serene {
class SereneContext;

/// This class is quite similar to the `llvm::SourceMgr` in functionality. We
/// even borrowed some of the code from the original implementation but removed
/// a lot of code that ar irrelevant to us.
///
/// SouceMgr is responsible for finding a namespace in the `loadPaths` and read
/// the content of the `.srn` (or any of the `DEFAULT_SUFFIX`) into a
/// `llvm::MemoryBuffer` embedded in a `SrcBuffer` object as the owner of the
/// source files and then it will call the `reader` on the buffer to parse it
/// and create the actual `Namespace` object from the parsed AST.
///
/// Later on, whenever we need to refer to the source file of a namespace for
/// diagnosis purposes or any other purpose we can use the functions in this
/// class to get hold of a pointer to a specific `reader::Location` of the
/// buffer.
///
/// Note: Unlike the original version, SourceMgr does not handle the diagnostics
/// and it uses the Serene's `DiagnosticEngine` for that matter.
class SourceMgr {

public:
  // TODO: Make it a vector of supported suffixes
  std::string DEFAULT_SUFFIX = "srn";

private:
  struct SrcBuffer {
    /// The memory buffer for the file.
    std::unique_ptr<llvm::MemoryBuffer> buffer;

    /// Vector of offsets into Buffer at which there are line-endings
    /// (lazily populated). Once populated, the '\n' that marks the end of
    /// line number N from [1..] is at Buffer[OffsetCache[N-1]]. Since
    /// these offsets are in sorted (ascending) order, they can be
    /// binary-searched for the first one after any given offset (eg. an
    /// offset corresponding to a particular SMLoc).
    ///
    /// Since we're storing offsets into relatively small files (often smaller
    /// than 2^8 or 2^16 bytes), we select the offset vector element type
    /// dynamically based on the size of Buffer.
    mutable void *offsetCache = nullptr;

    /// Look up a given \p Ptr in in the buffer, determining which line it came
    /// from.
    unsigned getLineNumber(const char *ptr) const;
    template <typename T>
    unsigned getLineNumberSpecialized(const char *ptr) const;

    /// Return a pointer to the first character of the specified line number or
    /// null if the line number is invalid.
    const char *getPointerForLineNumber(unsigned lineNo) const;

    template <typename T>
    const char *getPointerForLineNumberSpecialized(unsigned lineNo) const;

    /// This is the location of the parent import or unknown location if it is
    /// the main namespace
    reader::LocationRange importLoc;

    SrcBuffer() = default;
    SrcBuffer(SrcBuffer &&);
    SrcBuffer(const SrcBuffer &) = delete;
    SrcBuffer &operator=(const SrcBuffer &) = delete;
    ~SrcBuffer();
  };

  /// This is all of the buffers that we are reading from.
  std::vector<SrcBuffer> buffers;

  /// A hashtable that works as an index from namespace names to the buffer
  /// position it the `buffer`
  llvm::StringMap<unsigned> nsTable;

  // This is the list of directories we should search for include files in.
  std::vector<std::string> loadPaths;

  bool isValidBufferID(unsigned i) const { return i && i <= buffers.size(); }

  /// Converts the ns name to a partial path by replacing the dots with slashes
  std::string inline convertNamespaceToPath(std::string ns_name);

public:
  SourceMgr()                  = default;
  SourceMgr(const SourceMgr &) = delete;
  SourceMgr &operator=(const SourceMgr &) = delete;
  SourceMgr(SourceMgr &&)                 = default;
  SourceMgr &operator=(SourceMgr &&) = default;
  ~SourceMgr()                       = default;

  /// Set the `loadPaths` to the given \p dirs. `loadPaths` is a vector of
  /// directories that Serene will look in order to find a file that constains a
  /// namespace which it is looking for.
  void setLoadPaths(const std::vector<std::string> &dirs) { loadPaths = dirs; }

  /// Return a reference to a `SrcBuffer` with the given ID \p i.
  const SrcBuffer &getBufferInfo(unsigned i) const {
    assert(isValidBufferID(i));
    return buffers[i - 1];
  }

  /// Return a reference to a `SrcBuffer` with the given namspace name \p ns.
  const SrcBuffer &getBufferInfo(llvm::StringRef ns) const {
    auto bufferId = nsTable.lookup(ns);

    if (bufferId == 0) {
      // No such namespace
      llvm_unreachable("couldn't find the src buffer for a namespace. It "
                       "should never happen.");
    }

    return buffers[bufferId - 1];
  }

  /// Return a pointer to the internal `llvm::MemoryBuffer` of the `SrcBuffer`
  /// with the given ID \p i.
  const llvm::MemoryBuffer *getMemoryBuffer(unsigned i) const {
    assert(isValidBufferID(i));
    return buffers[i - 1].buffer.get();
  }

  unsigned getNumBuffers() const { return buffers.size(); }

  /// Add a new source buffer to this source manager. This takes ownership of
  /// the memory buffer.
  unsigned AddNewSourceBuffer(std::unique_ptr<llvm::MemoryBuffer> f,
                              reader::LocationRange includeLoc);

  /// Lookup for a file containing the namespace definition of with given
  /// namespace name \p name and throw an error. In case that the file exists,
  /// use the parser to read the file and create an AST from it. Then create a
  /// namespace, set the its AST to the AST that we just read from the file and
  /// return a shared pointer to the namespace.
  ///
  /// \p importLoc is a location in the source code where the give namespace is
  /// imported.
  NSPtr readNamespace(SereneContext &ctx, std::string name,
                      reader::LocationRange importLoc);
};

}; // namespace serene

#endif