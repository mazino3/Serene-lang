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

#ifndef SERENE_ERRORS_ERRC_H
#define SERENE_ERRORS_ERRC_H

#include "llvm/Support/Errc.h"

namespace serene {

/// A collection of common error codes in Serene
enum class errc {
  argument_list_too_long  = int(std::errc::argument_list_too_long),
  argument_out_of_domain  = int(std::errc::argument_out_of_domain),
  bad_address             = int(std::errc::bad_address),
  bad_file_descriptor     = int(std::errc::bad_file_descriptor),
  broken_pipe             = int(std::errc::broken_pipe),
  device_or_resource_busy = int(std::errc::device_or_resource_busy),
  directory_not_empty     = int(std::errc::directory_not_empty),
  executable_format_error = int(std::errc::executable_format_error),
  file_exists             = int(std::errc::file_exists),
  file_too_large          = int(std::errc::file_too_large),
  filename_too_long       = int(std::errc::filename_too_long),
  function_not_supported  = int(std::errc::function_not_supported),
  illegal_byte_sequence   = int(std::errc::illegal_byte_sequence),
  inappropriate_io_control_operation =
      int(std::errc::inappropriate_io_control_operation),
  interrupted                   = int(std::errc::interrupted),
  invalid_argument              = int(std::errc::invalid_argument),
  invalid_seek                  = int(std::errc::invalid_seek),
  io_error                      = int(std::errc::io_error),
  is_a_directory                = int(std::errc::is_a_directory),
  no_child_process              = int(std::errc::no_child_process),
  no_lock_available             = int(std::errc::no_lock_available),
  no_space_on_device            = int(std::errc::no_space_on_device),
  no_such_device_or_address     = int(std::errc::no_such_device_or_address),
  no_such_device                = int(std::errc::no_such_device),
  no_such_file_or_directory     = int(std::errc::no_such_file_or_directory),
  no_such_process               = int(std::errc::no_such_process),
  not_a_directory               = int(std::errc::not_a_directory),
  not_enough_memory             = int(std::errc::not_enough_memory),
  not_supported                 = int(std::errc::not_supported),
  operation_not_permitted       = int(std::errc::operation_not_permitted),
  permission_denied             = int(std::errc::permission_denied),
  read_only_file_system         = int(std::errc::read_only_file_system),
  resource_deadlock_would_occur = int(std::errc::resource_deadlock_would_occur),
  resource_unavailable_try_again =
      int(std::errc::resource_unavailable_try_again),
  result_out_of_range           = int(std::errc::result_out_of_range),
  too_many_files_open_in_system = int(std::errc::too_many_files_open_in_system),
  too_many_files_open           = int(std::errc::too_many_files_open),
  too_many_links                = int(std::errc::too_many_links)
};

/// The **official way** to create `std::error_code` in context of Serene.
inline std::error_code make_error_code(errc E) {
  return std::error_code(static_cast<int>(E), std::generic_category());
};

}; // namespace serene

#endif
