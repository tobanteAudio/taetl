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

#ifndef TAETL_STRING_VIEW_HPP
#define TAETL_STRING_VIEW_HPP

#include "definitions.hpp"
#include "string.hpp"

namespace etl
{
/**
 * @brief The class template basic_string_view describes an object that can
 * refer to a constant contiguous sequence of char-like objects with the first
 * element of the sequence at position zero. A typical implementation holds only
 * two members: a pointer to constant CharType and a size.
 */
template <class CharType, class Traits = etl::char_traits<CharType>>
class basic_string_view
{
public:
    using traits_type     = Traits;
    using value_type      = CharType;
    using pointer         = CharType*;
    using const_pointer   = CharType const*;
    using reference       = CharType&;
    using const_reference = CharType const&;
    using const_iterator  = CharType const*;
    using iterator        = const_iterator;
    using size_type       = etl::size_t;
    using difference_type = etl::ptrdiff_t;
    // using const_reverse_iterator = etl::reverse_iterator<const_iterator>;
    // using reverse_iterator       = const_reverse_iterator;

    /**
     * @brief Default constructor. Constructs an empty basic_string_view. After
     * construction, data() is equal to nullptr, and size() is equal to 0.
     */
    constexpr basic_string_view() noexcept = default;

    /**
     * @brief Copy constructor. Constructs a view of the same content as other.
     * After construction, data() is equal to other.data(), and size() is equal
     * to other.size().
     */
    constexpr basic_string_view(
        basic_string_view const& other) noexcept = default;

    /**
     * @brief Constructs a view of the first count characters of the character
     * array starting with the element pointed by s. s can contain null
     * characters. The behavior is undefined if [s, s+count) is not a valid
     * range (even though the constructor may not access any of the elements of
     * this range). After construction, data() is equal to s, and size() is
     * equal to count.
     */
    constexpr basic_string_view(CharType const* str, size_type size)
        : begin_ {str}, size_ {size}
    {
    }

    /**
     * @brief Constructs a view of the null-terminated character string pointed
     * to by s, not including the terminating null character. The length of the
     * view is determined as if by Traits::length(s). The behavior is undefined
     * if [s, s+Traits::length(s)) is not a valid range. After construction,
     * data() is equal to s, and size() is equal to Traits::length(s).
     */
    constexpr basic_string_view(CharType const* str)
        : begin_ {str}, size_ {traits_type::length(str)}
    {
    }

    /**
     * @brief Replaces the view with that of view.
     */
    [[nodiscard]] constexpr auto
    operator                  =(const basic_string_view& view) noexcept
        -> basic_string_view& = default;

    /**
     * @brief Returns an iterator to the first character of the view.
     */
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
    {
        return begin_;
    }

    /**
     * @brief Returns an iterator to the first character of the view.
     */
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
    {
        return begin_;
    }

    /**
     * @brief Returns an iterator to the character following the last character
     * of the view. This character acts as a placeholder, attempting to access
     * it results in undefined behavior.
     */
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator
    {
        return begin_ + size_;
    }

    /**
     * @brief Returns an iterator to the character following the last character
     * of the view. This character acts as a placeholder, attempting to access
     * it results in undefined behavior.
     */
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator
    {
        return begin_ + size_;
    }

    /**
     * @brief Returns a const reference to the character at specified location
     * pos. No bounds checking is performed: the behavior is undefined if pos >=
     * size().
     */
    [[nodiscard]] constexpr auto at(size_type pos) const -> const_reference
    {
        return begin_[pos];
    }

    /**
     * @brief Returns a const reference to the character at specified location
     * pos. No bounds checking is performed: the behavior is undefined if pos >=
     * size().
     */
    [[nodiscard]] constexpr auto operator[](size_type pos) const
        -> const_reference
    {
        return at(pos);
    }

    /**
     * @brief Returns reference to the first character in the view. The behavior
     * is undefined if empty() == true.
     */
    [[nodiscard]] constexpr auto front() const -> const_reference
    {
        return *begin_;
    }

    /**
     * @brief Returns reference to the last character in the view. The behavior
     * is undefined if empty() == true.
     */
    [[nodiscard]] constexpr auto back() const -> const_reference
    {
        return begin_[size_ - 1];
    }

    /**
     * @brief Returns a pointer to the underlying character array. The pointer
     * is such that the range [data(); data() + size()) is valid and the values
     * in it correspond to the values of the view.
     */
    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer
    {
        return begin_;
    }

    /**
     * @brief Returns the number of CharT elements in the view, i.e.
     * etl::distance(begin(), end()).
     */
    [[nodiscard]] constexpr auto size() const noexcept -> size_type
    {
        return size_;
    }

    /**
     * @brief Returns the number of CharT elements in the view, i.e.
     * etl::distance(begin(), end()).
     */
    [[nodiscard]] constexpr auto length() const noexcept -> size_type
    {
        return size_;
    }

    /**
     * @brief The largest possible number of char-like objects that can be
     * referred to by a basic_string_view.
     */
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type
    {
        return size_type(-1);
    }

    /**
     * @brief Checks if the view has no characters, i.e. whether size() == 0.
     */
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return size_ == 0;
    }

    /**
     * @brief Moves the start of the view forward by n characters. The behavior
     * is undefined if n > size().
     */
    constexpr auto remove_prefix(size_type n) -> void
    {
        begin_ += n;
        size_ -= n;
    }

    /**
     * @brief Moves the end of the view back by n characters. The behavior is
     * undefined if n > size().
     */
    constexpr auto remove_suffix(size_type n) -> void { size_ = size_ - n; }

private:
    const_pointer begin_ = nullptr;
    size_type size_      = 0;
};

/**
 * @brief Typedefs for common character type
 */
using string_view = basic_string_view<char, etl::char_traits<char>>;

}  // namespace etl

#endif  // TAETL_STRING_VIEW_HPP