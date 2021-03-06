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

#include "etl/mutex.hpp"
#include "etl/warning.hpp"

namespace
{
class dummy_mutex
{
  public:
  dummy_mutex() noexcept  = default;
  ~dummy_mutex() noexcept = default;

  auto operator=(dummy_mutex const&) -> dummy_mutex& = delete;
  dummy_mutex(dummy_mutex const&)                    = delete;

  auto operator=(dummy_mutex&&) -> dummy_mutex& = default;
  dummy_mutex(dummy_mutex&&)                    = default;

  auto lock() noexcept { is_locked_ = true; }
  auto unlock() noexcept { is_locked_ = false; }

  [[nodiscard]] auto is_locked() const noexcept { return is_locked_; }

  private:
  bool is_locked_ = false;
};
}  // namespace

TEST_CASE("mutex/lock_guard: construct", "[mutex]")
{
  SECTION("not locked")
  {
    dummy_mutex mtx {};
    REQUIRE_FALSE(mtx.is_locked());
    {
      etl::lock_guard lock {mtx};
      REQUIRE(mtx.is_locked());
      etl::ignore_unused(lock);
    }
    REQUIRE_FALSE(mtx.is_locked());
  }

  SECTION("already locked")
  {
    dummy_mutex mtx {};
    mtx.lock();
    {
      etl::lock_guard lock {mtx, etl::adopt_lock};
      REQUIRE(mtx.is_locked());
      etl::ignore_unused(lock);
    }
    REQUIRE_FALSE(mtx.is_locked());
  }
}

TEST_CASE("mutex/scoped_lock: construct", "[mutex]")
{
  dummy_mutex mtx {};
  etl::scoped_lock lock {mtx};
  REQUIRE(mtx.is_locked());
  etl::ignore_unused(lock);
}

TEST_CASE("mutex/scoped_lock: lock/unlock", "[mutex]")
{
  dummy_mutex mtx {};
  REQUIRE_FALSE(mtx.is_locked());
  {
    etl::scoped_lock lock {mtx};
    REQUIRE(mtx.is_locked());
  }
  REQUIRE_FALSE(mtx.is_locked());
}
