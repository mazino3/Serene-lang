/*
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

#include "serene/fs.h"

namespace serene::fs {

std::string extensionFor(NSFileType t) {
  // TODO: Create a mapping from OS type and NSFileTypes and
  // append the appropriate extension to the path. For now
  // only few that we need are enough.
  switch (t) {
  case NSFileType::Source:
    return ".srn";
  case NSFileType::TextIR:
    return ".ll";
    break;
  case NSFileType::StaticLib:
    return ".a";
    break;
  default:
    // TODO: This is temporary, remove this when created
    // the mapping
    return ".unknown";
  };
};

/// Converts the given namespace name `nsName` to the file name
/// for that name space. E.g, `some.random.ns` will be translated
/// to `some_random_ns`.
std::string namespaceToPath(std::string &nsName) {
  std::replace(nsName.begin(), nsName.end(), '.', '/');

  llvm::SmallString<MAX_PATH_SLOTS> path;
  path.append(nsName);
  llvm::sys::path::native(path);

  return std::string(path);
};

/// Return a boolean indicating whether or not the given path exists.
bool exists(llvm::StringRef path) {
  llvm::sys::fs::file_status status;
  auto err = llvm::sys::fs::status(path, status);

  if (err) {
    return false;
  };

  return llvm::sys::fs::exists(status);
}

/// Join the given `path1` and `path2` with respect to the platform
/// conventions.
std::string join(llvm::StringRef path1, llvm::StringRef path2) {
  llvm::SmallString<MAX_PATH_SLOTS> path;
  path.append(path1);
  path.append(llvm::sys::path::get_separator().data());
  path.append(path2);
  llvm::sys::path::native(path);
  return std::string(path);
};

}; // namespace serene::fs
