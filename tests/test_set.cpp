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

#include "etl/set.hpp"

#include "etl/array.hpp"
#include "etl/string.hpp"

TEMPLATE_TEST_CASE("set/static_set: typedefs", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  using set_t = etl::static_set<TestType, 16>;

  STATIC_REQUIRE(etl::is_same_v<TestType, typename set_t::value_type>);
  STATIC_REQUIRE(etl::is_same_v<TestType&, typename set_t::reference>);
  STATIC_REQUIRE(
    etl::is_same_v<TestType const&, typename set_t::const_reference>);
  STATIC_REQUIRE(etl::is_same_v<TestType*, typename set_t::pointer>);
  STATIC_REQUIRE(
    etl::is_same_v<TestType const*, typename set_t::const_pointer>);
  STATIC_REQUIRE(etl::is_same_v<TestType*, typename set_t::iterator>);
  STATIC_REQUIRE(
    etl::is_same_v<TestType const*, typename set_t::const_iterator>);
}

TEMPLATE_TEST_CASE("set/static_set: ctor(default)", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  SECTION("0")
  {
    auto set = etl::static_set<TestType, 0>();
    CHECK(set.size() == 0);
    CHECK(set.max_size() == 0);
    CHECK(set.empty());
    CHECK(set.full());
    CHECK(set.begin() == nullptr);
    CHECK(etl::as_const(set).begin() == nullptr);
    CHECK(set.end() == nullptr);
    CHECK(etl::as_const(set).end() == nullptr);
  }

  SECTION("4")
  {
    auto set = etl::static_set<TestType, 4>();
    CHECK(set.size() == 0);
    CHECK(set.max_size() == 4);
    CHECK(set.empty());
    CHECK_FALSE(set.full());
  }

  SECTION("16")
  {
    auto set = etl::static_set<TestType, 16>();
    CHECK(set.size() == 0);
    CHECK(set.max_size() == 16);
    CHECK(set.empty());
    CHECK_FALSE(set.full());
  }
}

TEMPLATE_TEST_CASE("set/static_set: ctor(first,last)", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  using T   = TestType;
  auto data = etl::array {T(2), T(1), T(0), T(1)};
  auto set  = etl::static_set<TestType, 4>(begin(data), end(data));
  CHECK(set.size() == 3);
  CHECK(set.max_size() == 4);
  CHECK_FALSE(set.empty());
  CHECK_FALSE(set.full());
}

TEMPLATE_TEST_CASE("set/static_set: begin/end", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  auto set = etl::static_set<TestType, 4>();
  CHECK(begin(set) == end(set));
  CHECK(begin(etl::as_const(set)) == end(etl::as_const(set)));
  CHECK(set.cbegin() == set.cend());

  set.emplace(TestType(0));
  CHECK(begin(set) != end(set));
  CHECK(begin(etl::as_const(set)) != end(etl::as_const(set)));
  CHECK(cbegin(set) != cend(set));

  for (auto& key : set) { CHECK(key == 0); }
  etl::for_each(begin(set), end(set), [](auto key) { CHECK(key == 0); });
}

TEMPLATE_TEST_CASE("set/static_set: rbegin/rend", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  auto set = etl::static_set<TestType, 4>();
  CHECK(rbegin(set) == rend(set));
  CHECK(rbegin(etl::as_const(set)) == rend(etl::as_const(set)));
  CHECK(set.crbegin() == set.crend());

  set.emplace(TestType(0));
  CHECK(rbegin(set) != rend(set));
  CHECK(rbegin(etl::as_const(set)) != rend(etl::as_const(set)));
  CHECK(crbegin(set) != crend(set));

  etl::for_each(rbegin(set), rend(set), [](auto key) { CHECK(key == 0); });

  set.emplace(TestType(2));
  set.emplace(TestType(1));
  auto it = set.rbegin();
  CHECK(*it++ == TestType(2));
  CHECK(*it++ == TestType(1));
  CHECK(*it++ == TestType(0));
  CHECK(it == rend(set));
}

TEMPLATE_TEST_CASE("set/static_set: clear", "[set]", etl::uint8_t, etl::int8_t,
                   etl::uint16_t, etl::int16_t, etl::uint32_t, etl::int32_t,
                   etl::uint64_t, etl::int64_t, float, double, long double)
{
  auto set = etl::static_set<TestType, 2>();
  set.emplace(TestType(1));
  set.emplace(TestType(4));
  CHECK(set.full());
  CHECK_FALSE(set.empty());

  set.clear();
  CHECK(set.empty());
  CHECK_FALSE(set.full());
}

TEMPLATE_TEST_CASE("set/static_set: emplace", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  auto set = etl::static_set<TestType, 4>();

  // first element
  set.emplace(TestType(1));
  CHECK(set.contains(1));
  CHECK(set.size() == 1);
  CHECK_FALSE(set.empty());
  CHECK_FALSE(set.full());

  // in order, no reordering required
  set.emplace(TestType(2));
  CHECK(set.contains(2));
  CHECK(set.size() == 2);
  CHECK_FALSE(set.empty());
  CHECK_FALSE(set.full());

  // not in order, reordering required!
  set.emplace(TestType(0));
  CHECK(set.contains(0));
  CHECK(set.size() == 3);
  CHECK(*set.begin() == 0);
  CHECK_FALSE(set.empty());
  CHECK_FALSE(set.full());

  // value already in set
  set.emplace(TestType(0));
  CHECK(set.contains(0));
  CHECK(set.size() == 3);
  CHECK(*set.begin() == 0);
  CHECK_FALSE(set.empty());
  CHECK_FALSE(set.full());

  // last element
  CHECK(set.emplace(TestType(4)).second);
  CHECK(set.contains(4));
  CHECK(set.size() == 4);
  CHECK(*set.begin() == 0);
  CHECK(set.full());
  CHECK_FALSE(set.empty());

  // fails, capacity is reached.
  auto res = set.emplace(TestType(5));
  CHECK(res.first == nullptr);
  CHECK(res.second == false);
  CHECK(set.size() == 4);
  CHECK_FALSE(set.contains(5));
}

TEMPLATE_TEST_CASE("set/static_set: erase", "[set]", etl::uint8_t, etl::int8_t,
                   etl::uint16_t, etl::int16_t, etl::uint32_t, etl::int32_t,
                   etl::uint64_t, etl::int64_t, float, double, long double)
{
  using T   = TestType;
  auto data = etl::array {T(1), T(2), T(3), T(4)};
  auto set  = etl::static_set<T, 4>(begin(data), end(data));

  CHECK(set.contains(T(3)));
  CHECK(set.erase(T(3)) == 1);
  CHECK(set.size() == 3);
  CHECK_FALSE(set.contains(T(3)));

  CHECK(set.contains(T(1)));
  CHECK(set.erase(begin(set)) == begin(set) + 1);
  CHECK(set.size() == 2);
  CHECK_FALSE(set.contains(T(1)));

  // CHECK(set.contains(T(2)));
  // CHECK(set.erase(begin(set), end(set) - 1) == end(set));
  // CHECK(set.size() == 1);
  // CHECK_FALSE(set.contains(T(2)));
}

TEMPLATE_TEST_CASE("set/static_set: find", "[set]", etl::uint8_t, etl::int8_t,
                   etl::uint16_t, etl::int16_t, etl::uint32_t, etl::int32_t,
                   etl::uint64_t, etl::int64_t, float, double, long double)
{
  auto set = etl::static_set<TestType, 4>();
  CHECK(set.find(0) == end(set));

  set.emplace(TestType(0));
  CHECK(set.find(0) != end(set));
  CHECK(set.find(0) == begin(set));
  CHECK(set.find(1) == end(set));

  set.emplace(TestType(1));
  CHECK(set.find(0) != end(set));
  CHECK(set.find(1) != end(set));
  CHECK(set.find(1) == begin(set) + 1);
}

TEMPLATE_TEST_CASE("set/static_set: contains", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  auto set = etl::static_set<TestType, 4>();
  CHECK_FALSE(set.contains(0));

  set.emplace(TestType(0));
  CHECK(set.contains(0));
  CHECK_FALSE(set.contains(1));

  set.emplace(TestType(1));
  CHECK(set.contains(0));
  CHECK(set.contains(1));
}

TEMPLATE_TEST_CASE("set/static_set: key_comp/value_comp", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  using T = TestType;

  auto set   = etl::static_set<TestType, 4>();
  auto k_cmp = set.key_comp();
  auto v_cmp = set.value_comp();

  // Compare functions hould be equal
  CHECK(k_cmp(T(), T()) == v_cmp(T(), T()));
  CHECK(k_cmp(T(1), T(1)) == v_cmp(T(1), T(1)));
  CHECK(k_cmp(T(1), T(2)) == v_cmp(T(1), T(2)));
  CHECK(k_cmp(T(2), T(1)) == v_cmp(T(2), T(1)));
}

TEMPLATE_TEST_CASE("set/static_set: swap", "[set]", etl::uint8_t, etl::int8_t,
                   etl::uint16_t, etl::int16_t, etl::uint32_t, etl::int32_t,
                   etl::uint64_t, etl::int64_t, float, double, long double)
{
  using T = TestType;
  using etl::swap;

  SECTION("empty")
  {
    auto lhs = etl::static_set<TestType, 4>();
    auto rhs = etl::static_set<TestType, 4>();
    CHECK(lhs.empty());
    CHECK(rhs.empty());

    swap(lhs, rhs);
    CHECK(lhs.empty());
    CHECK(rhs.empty());

    rhs.swap(lhs);
    CHECK(lhs.empty());
    CHECK(rhs.empty());
  }

  SECTION("same size")
  {
    auto lhs_data = etl::array {T(1), T(2), T(3)};
    auto rhs_data = etl::array {T(4), T(5), T(6)};
    auto lhs      = etl::static_set<T, 4>(begin(lhs_data), end(lhs_data));
    auto rhs      = etl::static_set<T, 4>(begin(rhs_data), end(rhs_data));
    CHECK(lhs.size() == rhs.size());
    CHECK(*lhs.begin() == T(1));
    CHECK(*rhs.begin() == T(4));

    lhs.swap(rhs);
    CHECK(lhs.size() == rhs.size());
    CHECK(*lhs.begin() == T(4));
    CHECK(*rhs.begin() == T(1));

    swap(rhs, lhs);
    CHECK(lhs.size() == rhs.size());
    CHECK(*lhs.begin() == T(1));
    CHECK(*rhs.begin() == T(4));
  }

  SECTION("different size")
  {
    auto lhs_data = etl::array {T(1), T(2), T(3)};
    auto rhs_data = etl::array {T(4), T(5)};
    auto lhs      = etl::static_set<T, 4>(begin(lhs_data), end(lhs_data));
    auto rhs      = etl::static_set<T, 4>(begin(rhs_data), end(rhs_data));
    CHECK(lhs.size() == 3);
    CHECK(rhs.size() == 2);
    CHECK(*lhs.begin() == T(1));
    CHECK(*rhs.begin() == T(4));

    lhs.swap(rhs);
    CHECK(lhs.size() == 2);
    CHECK(rhs.size() == 3);
    CHECK(*lhs.begin() == T(4));
    CHECK(*rhs.begin() == T(1));

    swap(rhs, lhs);
    CHECK(lhs.size() == 3);
    CHECK(rhs.size() == 2);
    CHECK(*lhs.begin() == T(1));
    CHECK(*rhs.begin() == T(4));
  }
}

TEMPLATE_TEST_CASE("set/static_set: operator==/!=", "[set]", etl::uint8_t,
                   etl::int8_t, etl::uint16_t, etl::int16_t, etl::uint32_t,
                   etl::int32_t, etl::uint64_t, etl::int64_t, float, double,
                   long double)
{
  SECTION("empty")
  {
    auto lhs = etl::static_set<TestType, 4>();
    auto rhs = etl::static_set<TestType, 4>();
    CHECK(lhs == rhs);
    CHECK(rhs == lhs);
    CHECK(etl::as_const(lhs) == etl::as_const(rhs));
    CHECK(etl::as_const(rhs) == etl::as_const(lhs));

    CHECK_FALSE(lhs != rhs);
    CHECK_FALSE(rhs != lhs);
    CHECK_FALSE(etl::as_const(lhs) != etl::as_const(rhs));
    CHECK_FALSE(etl::as_const(rhs) != etl::as_const(lhs));
  }

  SECTION("equal")
  {
    auto data = etl::array {TestType(1), TestType(2), TestType(3)};
    auto lhs  = etl::static_set<TestType, 4>(begin(data), end(data));
    auto rhs  = etl::static_set<TestType, 4>(begin(data), end(data));

    CHECK(lhs == rhs);
    CHECK(rhs == lhs);
    CHECK(etl::as_const(lhs) == etl::as_const(rhs));
    CHECK(etl::as_const(rhs) == etl::as_const(lhs));

    CHECK_FALSE(lhs != rhs);
    CHECK_FALSE(rhs != lhs);
    CHECK_FALSE(etl::as_const(lhs) != etl::as_const(rhs));
    CHECK_FALSE(etl::as_const(rhs) != etl::as_const(lhs));
  }

  SECTION("not equal")
  {
    auto data = etl::array {TestType(1), TestType(2), TestType(3)};
    auto lhs  = etl::static_set<TestType, 4>(begin(data), end(data) - 1);
    auto rhs  = etl::static_set<TestType, 4>(begin(data), end(data));

    CHECK(lhs != rhs);
    CHECK(rhs != lhs);
    CHECK(etl::as_const(lhs) != etl::as_const(rhs));
    CHECK(etl::as_const(rhs) != etl::as_const(lhs));

    CHECK_FALSE(lhs == rhs);
    CHECK_FALSE(rhs == lhs);
    CHECK_FALSE(etl::as_const(lhs) == etl::as_const(rhs));
    CHECK_FALSE(etl::as_const(rhs) == etl::as_const(lhs));
  }
}

// TEMPLATE_TEST_CASE("set/static_set: erase_if", "[set]", etl::uint8_t,
// etl::int8_t,
//                    etl::uint16_t, etl::int16_t, etl::uint32_t, etl::int32_t,
//                    etl::uint64_t, etl::int64_t, float, double, long double)
// {
//     auto predicate = [](auto const& val) { return val == TestType {1}; };

//     SECTION("empty")
//     {
//         auto set = etl::static_set<TestType, 4>();
//         CHECK(set.empty());
//         CHECK_FALSE(set.contains(TestType {1}));
//         CHECK(etl::erase_if(set, predicate) == 0);
//         CHECK(set.empty());
//         CHECK_FALSE(set.contains(TestType {1}));
//     }

//     SECTION("not empty")
//     {
//         auto data = etl::array {TestType(1), TestType(2), TestType(3)};
//         auto set  = etl::static_set<TestType, 4>();
//         CHECK(set.contains(TestType {1}));
//         CHECK(etl::erase_if(set, predicate) == 0);
//         CHECK_FALSE(set.contains(TestType {1}));
//     }
// }