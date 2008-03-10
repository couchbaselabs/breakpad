// Copyright (c) 2007, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// DiskModuleCache implements SourceLineResolverModuleCacheInterface,
// storing the cached objects on disk.

#ifndef GOOGLE_BREAKPAD_PROCESSOR_DISK_MODULE_CACHE_H__
#define GOOGLE_BREAKPAD_PROCESSOR_DISK_MODULE_CACHE_H__

#include "google_breakpad/processor/source_line_resolver_module_cache_interface.h"

namespace google_breakpad {

using std::string;
using std::istream;
using std::ostream;

class DiskModuleCache : public SourceLineResolverModuleCacheInterface {
 public:
  DiskModuleCache(string cache_directory);
  virtual ~DiskModuleCache() {}

  // Retrieve the symbol data associated with a module
  virtual bool GetModuleData(const string &symbol_file,
                             istream **data_stream);

  // Get a stream to which the data associated with a module
  // can be stored
  virtual bool BeginSetModuleData(const string &symbol_file,
                                  ostream **data_stream);
  // Finish setting the data associated with this module,
  // data should already have been written to the stream
  virtual bool EndSetModuleData(const string &symbol_file,
                                ostream **data_stream);

 private:
  string cache_directory_;
  // Given a symbol file, map to a cache entry on disk
  string MapToCacheEntry(const string &symbol_file);
  // Given a file path, ensure that all directories in the path exist.
  bool EnsurePathExists(const string &cache_entry);
  // determine if this path is a directory
  bool IsDir(const string &path);
};

} // namespace google_breakpad

#endif  // GOOGLE_BREAKPAD_PROCESSOR_DISK_MODULE_CACHE_H__
