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

#ifndef TAETL_BIT_HPP
#define TAETL_BIT_HPP

#include "etl/definitions.hpp"
#include "etl/type_traits.hpp"

namespace etl
{
/**
 * @brief Indicates the endianness of all scalar types.
 *
 * @details If all scalar types are little-endian, etl::endian::native equals
 * etl::endian::little. If all scalar types are big-endian,
 * etl::endian::native equals etl::endian::big
 */
enum class endian
{
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

/**
 * @brief Returns the number of 1 bits in the value of x.
 *
 * @details This overload only participates in overload resolution if T is an
 * unsigned integer type (that is, unsigned char, unsigned short, unsigned int,
 * unsigned long, unsigned long long, or an extended unsigned integer type).
 *
 */
template <class T>
[[nodiscard]] constexpr auto popcount(T input) noexcept
    -> etl::enable_if_t<etl::is_unsigned_v<T>, int>
{
    auto count = T {0};
    while (input)
    {
        count = count + (input & T {1});
        input = input >> T {1};
    }
    return static_cast<int>(count);
}

/**
 * @brief Checks if x is an integral power of two.
 *
 * @details This overload only participates in overload resolution if T is an
 * unsigned integer type (that is, unsigned char, unsigned short, unsigned int,
 * unsigned long, unsigned long long, or an extended unsigned integer type).
 *
 * @return true if x is an integral power of two; otherwise false.
 */
template <typename T>
constexpr auto has_single_bit(T x) noexcept -> enable_if_t<
    !is_same_v<
        T,
        bool> && !is_same_v<T, char> && !is_same_v<T, char16_t> && !is_same_v<T, char32_t> && !is_same_v<T, wchar_t>,
    bool>
{
    return popcount(x) == 1;
}
}  // namespace etl

#endif  // TAETL_BIT_HPP