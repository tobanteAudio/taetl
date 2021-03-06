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

#include "catch2/catch_template_test_macros.hpp"

#include "etl/cstdint.hpp"  // for int16_t, int32_t, int64_t, int8_t
#include "etl/tuple.hpp"    // for get, tuple
#include "etl/warning.hpp"  // for ignore_unused

TEMPLATE_TEST_CASE("tuple: constructor", "[tuple]", bool, etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)

{
  etl::tuple<TestType, float> t1 {TestType {1}, 2.0F};
  etl::ignore_unused(t1);
}

TEMPLATE_TEST_CASE("tuple: get", "[tuple]", bool, etl::uint8_t, etl::int8_t,
                   etl::uint16_t, etl::int16_t, etl::uint32_t, etl::int32_t,
                   etl::uint64_t, etl::int64_t, float, double, long double)
{
  auto t1 = etl::tuple<TestType, float> {TestType {1}, 2.0F};
  CHECK(etl::get<0>(t1) == TestType {1});
  CHECK(etl::get<1>(t1) == 2.0F);
}

// TEMPLATE_TEST_CASE("tuple: tie", "[tuple]", bool, etl::uint8_t, etl::int8_t,
//                    etl::uint16_t, etl::int16_t, etl::uint32_t, etl::int32_t,
//                    etl::uint64_t, etl::int64_t, float, double, long double)
// {
//     auto data      = etl::tuple<TestType, float> {TestType {1}, 2.0F};
//     auto f         = float {};
//     auto t         = TestType {};
//     etl::tie(t, f) = data;
//     CHECK(t == TestType {1});
//     CHECK(f == 2.0F);
// }
