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

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <fstream>
#include <ios>
#include <vector>

#include "processor/disk_module_cache.h"
#include "processor/logging.h"

using std::string;
using std::ostream;
using std::ofstream;
using std::istream;
using std::ifstream;
using std::ios;
using std::vector;


namespace google_breakpad {

// tempofstream writes data to a temporary file in the same
// directory as the file passed to the constructor.
// when closed, it renames the temporary file to the given filename.
class tempofstream : public ofstream
{
public:
  tempofstream(const char * filename, ios_base::openmode mode = ios_base::out);
  void close();

private:
  string tempname_;
  string filename_;
};

tempofstream::tempofstream(const char * filename,
			   ios_base::openmode mode) : tempname_(""),
						      filename_(filename)
{
  string name_template_s = filename_ + "XXXXXX";
  vector<char> name_template(name_template_s.length() + 1);
  std::copy(name_template_s.begin(), name_template_s.end(),
	    name_template.begin());
  name_template[name_template.size() - 1] = '\0';
  tempname_ = mktemp(&name_template[0]);
  open(tempname_.c_str(), mode);
}

void tempofstream::close()
{
  ofstream::close();
  rename(tempname_.c_str(), filename_.c_str());
}

DiskModuleCache::DiskModuleCache(string cache_directory) :
    cache_directory_(cache_directory)
{
  // ensure trailing slash in cache directory
  if (cache_directory_[cache_directory_.length()] != '/')
    cache_directory.append("/");
}

bool DiskModuleCache::GetModuleData(const string &symbol_file,
                                    istream **data_stream)
{
  if (!data_stream)
    return false;

  string cache_file = MapToCacheEntry(symbol_file);
  if (cache_file.empty())
    return false;

  *data_stream = new ifstream(cache_file.c_str(),
                              ios::in | ios::binary);
  if (!**data_stream) {
    delete *data_stream;
    *data_stream = NULL;

    BPLOG(INFO) << "Symbol file " << symbol_file << " not cached";
    return false;
  }
  BPLOG(INFO) << "Loading cached copy of symbol file " << symbol_file
              << " from " << cache_file;
  return true;
}

bool DiskModuleCache::BeginSetModuleData(const string &symbol_file,
                                         ostream **data_stream)
{
  if (!data_stream)
    return false;

  string cache_file = MapToCacheEntry(symbol_file);
  BPLOG(INFO) << "Writing cache entry " << cache_file;
  if (!EnsurePathExists(cache_file.substr(0, cache_file.rfind('/'))))
    return false;
  
  *data_stream = new tempofstream(cache_file.c_str(),
				  ios::out | ios::binary | ios::trunc);
  if (!**data_stream) {
    delete *data_stream;
    *data_stream = NULL;

    BPLOG(INFO) << "Failed writing cache entry " << cache_file;
    return false;
  }
  return true;
}

bool DiskModuleCache::EndSetModuleData(const string &symbol_file,
                                       ostream **data_stream)
{
  tempofstream *file_stream = dynamic_cast<tempofstream*>(*data_stream);
  if (!file_stream) {
    BPLOG(INFO) << "Error writing cache entry for " << symbol_file;
    return false;
  }
  
  BPLOG(INFO) << "Finished writing cache entry for " << symbol_file;
  file_stream->close();
  return true;
}

// We assume that symbol_file is in the Microsoft Symbol Server
// format, /path/debug_file/IDENTIFIER/debug_file.sym.  We map
// this to /cache/path/debug_file/IDENTIFIER/debug_file.symcache.
// NOTE: this assumes unix style paths!
string DiskModuleCache::MapToCacheEntry(const string &symbol_file)
{
  string::size_type pos = string::npos;

  // we want the last three components in the path
  for (int i=0; i<3; i++) {
    pos = symbol_file.rfind('/', pos);
    if (pos == string::npos)
      return "";
    // back up one character so we don't find this slash again
    pos--;
  }

  string cache_file(cache_directory_ +
                    symbol_file.substr(pos + 2));
  pos = cache_file.length() - 4;
  if (cache_file.compare(pos, 4, ".sym") == 0)
    cache_file.replace(pos, 4, ".symcache");

  return cache_file;
}

bool DiskModuleCache::EnsurePathExists(const string &path)
{
  if (IsDir(path))
    return true;

  if (mkdir(path.c_str(), 0755) == -1) {
    // we handle ENOENT here
    if (errno != ENOENT)
      return false;

    string::size_type pos = path.rfind('/');
    if (pos == string::npos)
      // not enough path left?
      return false;

    if (!EnsurePathExists(path.substr(0, pos)))
      return false;

    // all previous directories should exist at this point
    return mkdir(path.c_str(), 0755) != -1;
  }
  else {
    return true;
  }
}

bool DiskModuleCache::IsDir(const string &path)
{
  struct stat st;
  if (stat(path.c_str(), &st) != 0)
    return false;

  return (st.st_mode & S_IFDIR) == S_IFDIR;
}

}  // namespace google_breakpad
