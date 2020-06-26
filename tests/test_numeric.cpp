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

// TAETL
#include "taetl/numeric.hpp"
#include "taetl/vector.hpp"

#include "catch2/catch.hpp"

TEST_CASE("numeric: abs", "[numeric]")
{
    REQUIRE(taetl::abs(10) == 10);
    REQUIRE(taetl::abs(0) == 0);
    REQUIRE(taetl::abs(-10) == 10);
    REQUIRE(taetl::abs(1.0) == 1.0);
    REQUIRE(taetl::abs(-1.0) == 1.0);
}

TEST_CASE("numeric: accumulate", "[numeric]")
{
    taetl::make::vector<double, 16> vec;
    vec.push_back(1.0);
    vec.push_back(2.0);
    vec.push_back(3.0);
    vec.push_back(4.0);

    // accumulate
    REQUIRE(taetl::accumulate(vec.begin(), vec.end(), 0.0) == 10.0);

    // accumulate binary function op
    auto func = [](double a, double b) { return a + (b * 2); };
    REQUIRE(taetl::accumulate(vec.begin(), vec.end(), 0.0, func) == 20.0);
}
