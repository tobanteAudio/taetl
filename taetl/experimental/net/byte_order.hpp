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

#ifndef TAETL_NET_BYTE_ORDER_HPP
#define TAETL_NET_BYTE_ORDER_HPP

#include "taetl/definitions.hpp"

namespace taetl
{
namespace net
{
template <class T>
constexpr T ntoh(T) = delete;
constexpr char ntoh(char v) noexcept { return v; }
constexpr uint8_t ntoh(uint8_t v) noexcept { return v; }
constexpr int8_t ntoh(int8_t v) noexcept { return v; }
constexpr uint16_t ntoh(uint16_t v) noexcept { return (v << 8) | (v >> 8); }
constexpr uint32_t ntoh(uint32_t v) noexcept
{
    auto const a = v << 24;
    auto const b = (v & 0x0000FF00) << 8;
    auto const c = (v & 0x00FF0000) >> 8;
    auto const d = v >> 24;

    return a | b | c | d;
}

template <class T>
constexpr T hton(T) = delete;
constexpr char hton(char v) noexcept { return v; }
constexpr int8_t hton(int8_t v) noexcept { return v; }
constexpr uint8_t hton(uint8_t v) noexcept { return v; }
constexpr uint16_t hton(uint16_t v) noexcept { return ntoh(v); }
constexpr uint32_t hton(uint32_t v) noexcept { return ntoh(v); }

}  // namespace net
}  // namespace taetl

#endif  // TAETL_NET_BYTE_ORDER_HPP