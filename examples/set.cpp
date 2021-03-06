/*
Copyright (c) 2019-2020, Tobias Hienzsch
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#include <assert.h>  // for assert
#include <stdio.h>   // for printf

#include "etl/algorithm.hpp"  // for for_each
#include "etl/array.hpp"      // for array
#include "etl/set.hpp"        // for static_set

auto main() -> int
{
  // Basic usage
  etl::static_set<int, 16> set_1;
  set_1.insert(3);  // 3
  set_1.insert(1);  // 1, 3
  set_1.insert(2);  // 1, 2, 3
  set_1.insert(4);  // 1, 2, 3, 4
  set_1.insert(4);  // 1, 2, 3, 4

  etl::for_each(begin(set_1), end(set_1),
                [](auto key) { printf("%d\n", key); });

  assert(set_1.contains(2));
  assert(set_1.contains(5) == false);

  // Construct from range
  auto data  = etl::array {1.0f, 2.0f, 3.0f};
  auto set_2 = etl::static_set<float, 3> {begin(data), end(data)};

  assert(set_2.full());
  assert(set_2.size() == 3);
  assert(set_2.count(1.0f) == 1);

  return 0;
}
