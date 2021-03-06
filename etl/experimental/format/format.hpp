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

#ifndef TAETL_EXPERIMENTAL_FORMAT_HPP
#define TAETL_EXPERIMENTAL_FORMAT_HPP

#include "etl/cstdint.hpp"
#include "etl/cstring.hpp"
#include "etl/iterator.hpp"
#include "etl/string.hpp"
#include "etl/string_view.hpp"
#include "etl/vector.hpp"
#include "etl/warning.hpp"

namespace etl::experimental::format
{
/**
 * @brief The enabled specializations of formatter define formatting rules for a
 * given type. Enabled specializations meet the Formatter requirements.
 *
 * https://en.cppreference.com/w/cpp/utility/format/formatter
 */
template <typename T, typename CharT = char>
struct formatter;

/**
 * @brief Provides access to formatting state consisting of the formatting
 * arguments and the output iterator.
 *
 * The behavior is undefined if OutputIt does not model output_iterator<const
 * CharT&>.
 *
 * https://en.cppreference.com/w/cpp/utility/format/basic_format_context
 */
template <typename OutputIt, typename CharT>
class basic_format_context
{
  public:
  using iterator  = OutputIt;
  using char_type = CharT;

  explicit constexpr basic_format_context(OutputIt pos) noexcept
      : pos_ {pos} { }

  template <typename T>
  using formatter_type = formatter<T, CharT>;

  /**
   * @brief Returns the iterator to the output buffer.
   */
  [[nodiscard]] constexpr auto out() noexcept -> iterator { return pos_; }

  /**
   * @brief Sets the output iterator to it. After a call to advance_to,
   * subsequent calls to out() will return a copy of it.
   */
  constexpr auto advance_to(iterator it) noexcept -> void { pos_ = it; }

  private:
  OutputIt pos_;
};

/**
 * @brief Provides access to formatting state consisting of the formatting
 * arguments and the output iterator.
 *
 * @details The first template argument is an output iterator that appends to
 * etl::static_string, such as etl::back_insert_iterator<etl::static_string>.
 * Implementations are encouraged to use an iterator to type-erased buffer type
 * that supports appending to any contiguous and resizable container.
 *
 * The behavior is undefined if OutputIt does not model output_iterator<const
 * CharT&>.
 *
 * https://en.cppreference.com/w/cpp/utility/format/basic_format_context
 */
template <typename ContainerT>
using format_context
  = basic_format_context<etl::back_insert_iterator<ContainerT>, char>;

/**
 * @brief Standard specializations for basic type char.
 */
template <>
struct formatter<char, char>
{
  template <typename FormatContext>
  constexpr auto format(char val, FormatContext& fc) -> decltype(fc.out())
  {
    auto pos = fc.out();
    *pos     = val;
    return pos++;
  }
};

/**
 * @brief Standard specializations for basic type char const*.
 */
template <>
struct formatter<char const*, char>
{
  template <typename FormatContext>
  constexpr auto format(char const* val, FormatContext& fc)
    -> decltype(fc.out())
  {
    return etl::copy(val, val + etl::strlen(val), fc.out());
  }
};

/**
 * @brief Standard specializations for basic type char array.
 */
template <::etl::size_t N>
struct formatter<char[N], char>
{
  template <typename FormatContext>
  constexpr auto format(char const* val, FormatContext& fc)
    -> decltype(fc.out())
  {
    return etl::copy(val, val + N, fc.out());
  }
};

/**
 * @brief Standard specializations for etl::string_view.
 */
template <>
struct formatter<etl::string_view, char>
{
  template <typename FormatContext>
  constexpr auto format(etl::string_view str, FormatContext& fc)
    -> decltype(fc.out())
  {
    return etl::copy(begin(str), end(str), fc.out());
  }
};

/**
 * @brief Standard specializations for etl::static_string<Capacity>.
 */
template <etl::size_t Capacity>
struct formatter<etl::static_string<Capacity>, char>
{
  template <typename FormatContext>
  constexpr auto format(etl::static_string<Capacity> const& str,
                        FormatContext& fc) -> decltype(fc.out())
  {
    return formatter<::etl::string_view>().format(str, fc);
  }
};

template <typename It>
using diff_t =
  typename ::etl::iterator_traits<::etl::remove_cvref_t<It>>::difference_type;

namespace detail
{
// Escape tokens
inline constexpr auto token_begin = '{';
inline constexpr auto token_end   = '}';

template <typename ValueT, typename FormatContext>
auto format_argument(ValueT const& val, FormatContext& fc) -> decltype(fc.out())
{
  auto formatter_ = formatter<ValueT, char> {};
  return formatter_.format(val, fc);
}

inline auto split_at_next_argument(etl::string_view str)
  -> etl::pair<etl::string_view, etl::string_view>
{
  using size_type = etl::string_view::size_type;

  auto const* res = etl::find(begin(str), end(str), token_begin);
  if (res != end(str) && *etl::next(res) == token_end)
  {
    auto index  = static_cast<size_type>(etl::distance(begin(str), res));
    auto first  = str.substr(0, index);
    auto second = str.substr(index + 2);
    return etl::make_pair(first, second);
  }

  return etl::make_pair(str, etl::string_view {});
}

template <typename FormatContext>
auto format_escaped_sequences(::etl::string_view str, FormatContext& ctx)
  -> void
{
  // Loop as long as escaped sequences are found.
  auto const* first = begin(str);
  while (true)
  {
    // Find open sequence {{
    const auto* const open_first = ::etl::find(first, end(str), token_begin);
    const auto* const open_sec   = ::etl::next(open_first);
    auto const escape_start      = open_first != end(str)  //
                              && open_sec != end(str)      //
                              && *open_sec == token_begin;

    if (escape_start)
    {
      // Copy upto {{
      detail::format_argument(etl::string_view(first, open_first), ctx);

      // Find sequence }}
      auto const* close_first
        = ::etl::find(::etl::next(open_sec), end(str), token_end);
      auto const* close_sec = ::etl::next(close_first);
      auto escape_close     = close_first != end(str)  //
                          && close_sec != end(str)     //
                          && *close_sec == token_end;

      // Copy everything between {{ ... }}, but only one curly each.
      if (escape_close)
      {
        detail::format_argument(etl::string_view(open_sec, close_first + 1),
                                ctx);
        first = close_first + 2;
      }
      else
      {
        assert(false && "No closing }} found");
        return;
      }
    }
    else
    {
      // No more escaped sequence found, copy rest.
      detail::format_argument(etl::string_view(first, end(str)), ctx);
      return;
    }
  }
}

}  // namespace detail

/**
 * @brief Format args according to the format string fmt, and write the result
 * to the output iterator out.
 *
 * https://en.cppreference.com/w/cpp/utility/format/format_to
 */
template <typename OutputIt, typename... Args>
auto format_to(OutputIt out, etl::string_view fmt, Args const&... args)
  -> OutputIt
{
  // TODO: Make more generic. What about other string types.
  auto ctx = format_context<::etl::static_string<32>> {out};

  // Format leading text before the first argument.
  auto const slices = detail::split_at_next_argument(fmt);
  detail::format_escaped_sequences(slices.first, ctx);

  // Save rest of format string. Supress warning if format_to was called without
  // arguments.
  auto rest = slices.second;
  ::etl::ignore_unused(rest);

  (
    [&] {
      // Format argument
      detail::format_argument(args, ctx);

      // Split format text at next argument
      auto const rest_slices = detail::split_at_next_argument(rest);
      detail::format_escaped_sequences(rest_slices.first, ctx);

      // Save rest of format string for the next arguments
      rest = rest_slices.second;
    }(),
    ...);

  // Anything left over after the last argument.
  if (auto const trailing = detail::split_at_next_argument(rest);
      !trailing.first.empty())
  {
    detail::format_escaped_sequences(trailing.first, ctx);
    assert(trailing.second.empty());
  }

  return ctx.out();
}

/**
 * @brief etl::format_to_n_result has no base classes, or members other than
 * out, size and implicitly declared special member functions.
 *
 * https://en.cppreference.com/w/cpp/utility/format/format_to_n
 */
template <typename Out>
struct format_to_n_result
{
  Out out;
  diff_t<Out> size;
};

/**
 * @brief Format args according to the format string fmt, and write the result
 * to the output iterator out. At most n characters are written.
 *
 * https://en.cppreference.com/w/cpp/utility/format/format_to_n
 */
template <typename OutputIter, typename... Args>
auto format_to_n(OutputIter out, diff_t<OutputIter> n, ::etl::string_view fmt,
                 Args const&... args) -> format_to_n_result<OutputIter>
{
  ::etl::ignore_unused(n);

  auto indices = ::etl::static_vector<::etl::size_t, sizeof...(args)> {};
  auto result  = format_to_n_result<OutputIter> {out, {}};

  auto write_char = [&result](auto ch) {
    *result.out++ = ch;
    result.size++;
  };

  auto var_start = ::etl::size_t {};
  for (decltype(fmt)::size_type i {}; i < fmt.size(); ++i)
  {
    auto ch = fmt[i];
    if (ch == '{')
    {
      if (fmt[i + 1] == '{')
      {
        ++i;
        write_char('{');
        continue;
      }

      var_start = i;
      continue;
    }

    if (ch == '}')
    {
      if (fmt[i + 1] == '}')
      {
        ++i;
        write_char('}');
        continue;
      }

      indices.push_back(var_start);
      write_char('0');
      continue;
    }

    write_char(ch);
  }

  if (indices.size() > 0)
  {
    [[maybe_unused]] auto replace_char_at
      = [n](auto output, auto pos, char val) {
          ::etl::ignore_unused(n);
          // assert((long)pos < n);
          output[pos] = val;
        };

    [[maybe_unused]] typename decltype(indices)::size_type i {};
    (replace_char_at(out, indices[i++], args), ...);
  }

  return result;
}
}  // namespace etl::experimental::format

#endif  // TAETL_EXPERIMENTAL_FORMAT_HPP
