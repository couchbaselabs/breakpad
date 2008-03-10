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

// equals.h: Templatized functions for checking equivalence.
//
// Use these instead of operator== so that we can check for equivalence
// of objects stored as pointers inside linked_ptrs.  Equals knows
// how to compare certain objects (RangeMap, AddressMap, ContainedRangeMap),
// and special-cases linked_ptr as well.  It defaults to operator== for
// everything else.
//
// Author: Ted Mielczarek

#ifndef GOOGLE_BREAKPAD_EQUALS_H_
#define GOOGLE_BREAKPAD_EQUALS_H_

#include <map>

#ifdef __SUNPRO_CC
#define BSLR_NO_HASH_MAP
#endif  // __SUNPRO_CC

#ifndef BSLR_NO_HASH_MAP
#include <ext/hash_map>
#endif

#include "google_breakpad/processor/basic_source_line_resolver.h"
#include "processor/linked_ptr.h"
#include "processor/range_map.h"
#include "processor/address_map.h"
#include "processor/contained_range_map.h"
#include "processor/logging.h"

namespace google_breakpad {

using std::map;
#ifndef BSLR_NO_HASH_MAP
using __gnu_cxx::hash_map;
#endif

// In the general case, just use operator==
// This should handle base types.
template<typename T>
inline bool Equals(const T &a, const T &b)
{
  return a == b;
}

// for linked_ptr, we want to compare the objects
// pointed at, not the raw pointers
template<typename U>
bool Equals(const linked_ptr<U> &a, const linked_ptr<U> &b)
{
  if (a.get() && b.get())
     return Equals(*a, *b);

  // if one is NULL, just compare pointers
  return a == b;
}

// for std::map/hash_map, we want to call Equals on each component of
// each pair
#ifndef BSLR_NO_HASH_MAP
template<typename k, typename v>
bool Equals(const hash_map<k, v> &a, const hash_map<k, v> &b)
{
  // simple check for equivalent size first
  if (a.size() != b.size())
    return false;

  typename hash_map<k,v>::const_iterator it, found;
  for (it = a.begin(); it != a.end(); it++) {
    // if b doesn't contain this entry, then
    // just get out
    found = b.find(it->first);
    if (found == b.end())
      return false;

    if (!Equals(it->second, found->second))
      return false;
  }
  return true;
}
#endif  // BSLR_NO_HASH_MAP

template<typename k, typename v>
bool Equals(const map<k, v> &a, const map<k, v> &b)
{
  // simple check for equivalent size first
  if (a.size() != b.size())
    return false;

  typename map<k,v>::const_iterator it, found;
  for (it = a.begin(); it != a.end(); it++) {
    // if b doesn't contain this entry, then
    // just get out
    found = b.find(it->first);
    if (found == b.end())
      return false;

    if (!Equals(it->second, found->second))
      return false;
  }
  return true;
}

// For certain classes we know about, use their Equals method.
// Note: a few variations of this are in other places:
// processor/basic_source_line_resolver.cc

template<typename AddressType, typename EntryType>
inline bool Equals(const RangeMap<AddressType, EntryType> &a,
            const RangeMap<AddressType, EntryType> &b)
{
  return a.Equals(b);
}

template<typename AddressType, typename EntryType>
inline bool Equals(const typename RangeMap<AddressType, EntryType>::Range &a,
                   const typename RangeMap<AddressType, EntryType>::Range &b)
{
  return a.Equals(b);
}

template<typename AddressType, typename EntryType>
inline bool Equals(const AddressMap<AddressType, EntryType> &a,
                   const AddressMap<AddressType, EntryType> &b)
{
  return a.Equals(b);
}

template<typename AddressType, typename EntryType>
inline bool Equals(const ContainedRangeMap<AddressType, EntryType> &a,
                   const ContainedRangeMap<AddressType, EntryType> &b)
{
  return a.Equals(b);
}

template<typename AddressType, typename EntryType>
inline bool Equals(ContainedRangeMap<AddressType, EntryType> * const &a,
                   ContainedRangeMap<AddressType, EntryType> * const &b)
{
  if (a && b)
    return a->Equals(*b);

  // if one is NULL, just compare pointer values
  return a == b;
}

} // namespace google_breakpad

#endif // GOOGLE_BREAKPAD_EQUALS_H_
