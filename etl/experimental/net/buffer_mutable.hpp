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

#ifndef TAETL_NET_BUFFER_MUTABLE_HPP
#define TAETL_NET_BUFFER_MUTABLE_HPP

#include "etl/array.hpp"
#include "etl/cstddef.hpp"

namespace etl
{
namespace net
{
class mutable_buffer
{
  public:
  /**
   * @brief Construct an empty buffer.
   */
  mutable_buffer() noexcept = default;

  /**
   * @brief Construct a buffer to represent a given memory range.
   */
  mutable_buffer(void* data, etl::size_t size) : data_ {data}, size_ {size} { }

  /**
   * @brief Get a pointer to the beginning of the memory range.
   */
  [[nodiscard]] auto data() const noexcept -> void* { return data_; }

  /**
   * @brief Get the size of the memory range.
   */
  [[nodiscard]] auto size() const noexcept -> etl::size_t { return size_; }

  /**
   * @brief Move the start of the buffer by the specified number of bytes.
   */
  auto operator+=(etl::size_t n) noexcept -> mutable_buffer&
  {
    auto const offset = n < size_ ? n : size_;
    data_             = static_cast<char*>(data_) + offset;
    size_ -= offset;
    return *this;
  }

  private:
  void* data_       = nullptr;
  etl::size_t size_ = 0;
};

/**
 * @brief Create a new modifiable buffer that is offset from the start of
 * another.
 * @relates mutable_buffer
 */
inline auto operator+(mutable_buffer const& b, etl::size_t const n) noexcept
  -> mutable_buffer
{
  auto offset = n < b.size() ? n : b.size();
  auto* data  = static_cast<char*>(b.data()) + offset;
  auto size   = b.size() - offset;
  return mutable_buffer {data, size};
}

/**
 * @brief Create a new modifiable buffer that is offset from the start of
 * another.
 * @relates mutable_buffer
 */
inline auto operator+(etl::size_t const n, mutable_buffer const& b) noexcept
  -> mutable_buffer
{
  return b + n;
}

}  // namespace net
}  // namespace etl

#endif  // TAETL_NET_BUFFER_MUTABLE_HPP
