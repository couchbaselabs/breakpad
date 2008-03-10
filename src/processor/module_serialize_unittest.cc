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

// module_serialize_unittest.cc: Unit tests for
// BasicSourceLineResolver::Module serialization.
//
// Author: Ted Mielczarek

#include <fstream>
#include "stdio.h"

#include "processor/logging.h"
#include "google_breakpad/processor/basic_source_line_resolver.h"

int main(int argc, char **argv) {
  BPLOG_INIT(&argc, &argv);

  char symbol_test_files[][1024] = {
    "src/processor/testdata/symbols/kernel32.pdb/BCE8785C57B44245A669896B6A19B9542/kernel32.sym",
    "src/processor/testdata/symbols/test_app.pdb/5A9832E5287241C1838ED98914E9B7FF1/test_app.sym"
  };

  for (int i=0; i < sizeof(symbol_test_files) / sizeof(symbol_test_files[0]);
       i++) {
    BPLOG(INFO) << "Testing round trip serialize for symbol file "
                << symbol_test_files[i];
    if (!google_breakpad::BasicSourceLineResolver::ModuleRoundTripTest(symbol_test_files[i])) {
      BPLOG(ERROR) << "FAILED: module round trip test for symbol file "
                   << symbol_test_files[i];
      return 1;
    }
  }

  return 0;
}
