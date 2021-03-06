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

#ifndef TAETL_CHRONO_HPP
#define TAETL_CHRONO_HPP

#include "etl/ratio.hpp"
#include "etl/type_traits.hpp"

#include "etl/detail/sfinae.hpp"

// Somehow the abs macro gets included in avr-gcc builds. Not sure where it's
// coming from.
#ifdef abs
#undef abs
#endif

namespace etl::chrono
{
/**
 * @brief The etl::chrono::duration_values type defines three common durations.
 *
 * @details The zero, min, and max methods in etl::chrono::duration forward
 * their work to these methods. This type can be specialized if the
 * representation Rep requires a specific implementation to return these
 * duration objects.
 */
template <typename Rep>
struct duration_values
{
  public:
  /**
   * @brief Returns a zero-length representation.
   */
  [[nodiscard]] static constexpr auto zero() -> Rep { return Rep {}; }

  /**
   * @brief Returns the smallest possible representation.
   */
  [[nodiscard]] static constexpr auto min() -> Rep
  {
    return etl::numeric_limits<Rep>::lowest();
  }

  /**
   * @brief Returns the special duration value max.
   */
  [[nodiscard]] static constexpr auto max() -> Rep
  {
    return etl::numeric_limits<Rep>::max();
  }
};

/**
 * @brief The etl::chrono::treat_as_floating_point trait helps determine if a
 * duration can be converted to another duration with a different tick period.
 *
 * @details Implicit conversions between two durations normally depends on the
 * tick period of the durations. However, implicit conversions can happen
 * regardless of tick period if
 * etl::chrono::treat_as_floating_point<Rep>::value == true.
 *
 * @note etl::chrono::treat_as_floating_point may be specialized for
 * program-defined types.
 */
template <typename Rep>
struct treat_as_floating_point : etl::is_floating_point<Rep>
{
};

template <typename Rep>
inline constexpr bool treat_as_floating_point_v
  = treat_as_floating_point<Rep>::value;

/**
 * @brief Class template etl::chrono::duration represents a time interval.
 *
 * @details It consists of a count of ticks of type Rep and a tick period, where
 * the tick period is a compile-time rational constant representing the number
 * of seconds from one tick to the next. The only data stored in a duration is a
 * tick count of type Rep. If Rep is floating point, then the duration can
 * represent fractions of ticks. Period is included as part of the duration's
 * type, and is only used when converting between different durations.
 */
template <typename Rep, typename Period = etl::ratio<1>>
class duration
{
  public:
  /**
   * @brief Rep, an arithmetic type representing the number of ticks.
   */
  using rep = Rep;

  /**
   * @brief A etl::ratio representing the tick period (i.e. the number of
   * seconds per tick).
   */
  using period = typename Period::type;

  /**
   * @brief Constructs a new duration from one of several optional data
   * sources. The default constructor is defaulted.
   */
  constexpr duration() noexcept = default;

  /**
   * @brief Constructs a new duration from one of several optional data
   * sources. The copy constructor is defaulted (makes a bitwise copy of the
   * tick count).
   */
  duration(duration const&) noexcept = default;

  /**
   * @brief Constructs a duration with r ticks.
   *
   * @details  Note that this constructor only participates in overload
   * resolution if const Rep2& (the argument type) is implicitly convertible
   * to rep (the type of this duration's ticks) and
   * treat_as_floating_point<rep>::value is true, or
   * treat_as_floating_point<Rep2>::value is false.
   *
   * That is, a duration with an integer tick count cannot be constructed from
   * a floating-point value, but a duration with a floating-point tick count
   * can be constructed from an integer value
   */
  template <
    typename Rep2,
    TAETL_REQUIRES_((is_convertible_v<Rep2, rep>)&&(
      treat_as_floating_point_v<rep> || !treat_as_floating_point_v<Rep2>))>
  constexpr explicit duration(Rep2 const& r) noexcept : data_(r)
  {
  }

  /**
   * @brief  Constructs a duration by converting d to an appropriate period
   * and tick count, as if by duration_cast<duration>(d).count().
   *
   * @details In order to prevent truncation during conversion, this
   * constructor only participates in overload resolution if computation of
   * the conversion factor (by etl::ratio_divide<Period2, Period>) does not
   * overflow and:
   *
   * treat_as_floating_point<rep>::value == true
   *
   * or both:
   *
   * ratio_divide<Period2, period>::den == 1, and
   * treat_as_floating_point<Rep2>::value == false
   *
   * That is, either the duration uses floating-point ticks, or Period2 is
   * exactly divisible by period
   */
  template <typename Rep2, typename Period2,
            TAETL_REQUIRES_((treat_as_floating_point_v<rep>)
                            || (ratio_divide<Period2, period>::den == 1
                                && !treat_as_floating_point_v<Rep2>))>
  constexpr duration(duration<Rep2, Period2> const& other) noexcept
      : data_(
        static_cast<Rep>(other.count() * ratio_divide<Period2, period>::num))
  {
  }

  /**
   * @brief Assigns the contents of one duration to another.
   */
  auto operator=(duration const& other) -> duration& = default;

  /**
   * @brief Returns the number of ticks for this duration.
   */
  [[nodiscard]] constexpr auto count() const -> rep { return data_; }

  /**
   * @brief Returns a zero-length duration.
   */
  [[nodiscard]] static constexpr auto zero() noexcept -> duration
  {
    return duration(etl::chrono::duration_values<rep>::zero());
  }

  /**
   * @brief Returns a duration with the lowest possible value.
   */
  [[nodiscard]] static constexpr auto min() noexcept -> duration
  {
    return duration(etl::chrono::duration_values<rep>::min());
  }

  /**
   * @brief Returns a duration with the largest possible value.
   */
  [[nodiscard]] static constexpr auto max() noexcept -> duration
  {
    return duration(etl::chrono::duration_values<rep>::max());
  }

  /**
   * @brief Implements unary plus and unary minus for the durations.
   */
  [[nodiscard]] constexpr auto operator+() const -> etl::common_type_t<duration>
  {
    return etl::common_type_t<duration>(*this);
  }

  /**
   * @brief Implements unary plus and unary minus for the durations.
   */
  [[nodiscard]] constexpr auto operator-() const -> etl::common_type_t<duration>
  {
    return etl::common_type_t<duration>(-data_);
  }

  /**
   * @brief Increments or decrements the number of ticks for this duration.
   * Equivalent to ++data_; return *this;
   */
  constexpr auto operator++() -> duration&
  {
    ++data_;
    return *this;
  }

  /**
   * @brief Increments or decrements the number of ticks for this duration.
   * Equivalent to return duration(data_++)
   */
  constexpr auto operator++(int) -> duration { return duration(data_++); }

  /**
   * @brief Increments or decrements the number of ticks for this duration.
   * Equivalent to --data_; return *this;
   */
  constexpr auto operator--() -> duration&
  {
    --data_;
    return *this;
  }

  /**
   * @brief Increments or decrements the number of ticks for this duration.
   * Equivalent to return duration(data_--);
   */
  constexpr auto operator--(int) -> duration { return duration(data_--); }

  private:
  rep data_ {};
};

/**
 * @brief Compares two durations. Checks if lhs and rhs are equal, i.e. the
 * number of ticks for the type common to both durations are equal.
 */
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
[[nodiscard]] constexpr auto operator==(duration<Rep1, Period1> const& lhs,
                                        duration<Rep2, Period2> const& rhs)
  -> bool
{
  using common_t = typename etl::common_type<duration<Rep1, Period1>,
                                             duration<Rep2, Period2>>::type;

  return common_t(lhs).count() == common_t(rhs).count();
}

/**
 * @brief Compares two durations. Checks if lhs and rhs are equal, i.e. the
 * number of ticks for the type common to both durations are equal.
 */
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
[[nodiscard]] constexpr auto operator!=(duration<Rep1, Period1> const& lhs,
                                        duration<Rep2, Period2> const& rhs)
  -> bool
{
  return !(lhs == rhs);
}

/**
 * @brief Compares two durations. Compares lhs to rhs, i.e. compares the number
 * of ticks for the type common to both durations.
 */
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
[[nodiscard]] constexpr auto operator<(duration<Rep1, Period1> const& lhs,
                                       duration<Rep2, Period2> const& rhs)
  -> bool
{
  using common_t = typename etl::common_type<duration<Rep1, Period1>,
                                             duration<Rep2, Period2>>::type;
  return common_t(lhs).count() < common_t(rhs).count();
}

/**
 * @brief Compares two durations. Compares lhs to rhs, i.e. compares the number
 * of ticks for the type common to both durations.
 */
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
[[nodiscard]] constexpr auto operator<=(duration<Rep1, Period1> const& lhs,
                                        duration<Rep2, Period2> const& rhs)
  -> bool
{
  return !(rhs < lhs);
}

/**
 * @brief Compares two durations. Compares lhs to rhs, i.e. compares the number
 * of ticks for the type common to both durations.
 */
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
[[nodiscard]] constexpr auto operator>(duration<Rep1, Period1> const& lhs,
                                       duration<Rep2, Period2> const& rhs)
  -> bool
{
  return rhs < lhs;
}

/**
 * @brief Compares two durations. Compares lhs to rhs, i.e. compares the number
 * of ticks for the type common to both durations.
 */
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
[[nodiscard]] constexpr auto operator>=(duration<Rep1, Period1> const& lhs,
                                        duration<Rep2, Period2> const& rhs)
  -> bool
{
  return !(lhs < rhs);
}

/**
 * @brief Signed integer type of at least 64 bits.
 */
using nanoseconds = duration<etl::int64_t, etl::nano>;

/**
 * @brief Signed integer type of at least 55 bits.
 */
using microseconds = duration<etl::int64_t, etl::micro>;

/**
 * @brief Signed integer type of at least 45 bits.
 */
using milliseconds = duration<etl::int64_t, etl::milli>;

/**
 * @brief Signed integer type of at least 35 bits.
 */
using seconds = duration<etl::int64_t>;

/**
 * @brief Signed integer type of at least 29 bits.
 */
using minutes = duration<etl::int32_t, etl::ratio<60>>;

/**
 * @brief Signed integer type of at least 23 bits.
 */
using hours = duration<etl::int32_t, etl::ratio<3600>>;

/**
 * @brief Signed integer type of at least 25 bits.
 */
using days = duration<etl::int32_t, etl::ratio<86'400>>;

/**
 * @brief Signed integer type of at least 22 bits.
 */
using weeks = duration<etl::int32_t, etl::ratio<604'800>>;

/**
 * @brief Signed integer type of at least 20 bits.
 */
using months = duration<etl::int32_t, etl::ratio<2'629'746>>;

/**
 * @brief Signed integer type of at least 17 bits.
 */
using years = duration<etl::int32_t, etl::ratio<31'556'952>>;

namespace detail
{
template <typename T>
struct is_duration : etl::false_type
{
};
template <typename Rep, typename Period>
struct is_duration<etl::chrono::duration<Rep, Period>> : etl::true_type
{
};
}  // namespace detail

/**
 * @brief Converts a etl::chrono::duration to a duration of different type
 * ToDuration.
 */
template <typename ToDuration, typename Rep, typename Period>
constexpr auto duration_cast(duration<Rep, Period> const& d) -> ToDuration
{
  return ToDuration(d);
}

// /**
//  * @brief Returns the greatest duration t representable in ToDuration that is
//  * less or equal to d. The function does not participate in the overload
//  * resolution unless ToDuration is an instance of etl::chrono::duration.
//  */
// template <typename To, typename Rep, typename Period,
//           typename = etl::enable_if_t<detail::is_duration<To>::value>>
// constexpr auto floor(duration <Rep, Period> const&   d) -> To
// {
//     auto const t = etl::chrono::duration_cast<To>(d);
//     if (t > d) { return t - To {1}; }
//     return t;
// }

/**
 * @brief Returns the absolute value of the duration d. Specifically, if d >=
 * d.zero(), return d, otherwise return -d. The function does not participate in
 * the overload resolution unless etl::numeric_limits<Rep>::is_signed is true.
 */
template <typename Rep, typename Period,
          TAETL_REQUIRES_(numeric_limits<Rep>::is_signed)>
constexpr auto abs(duration<Rep, Period> d) -> duration<Rep, Period>
{
  return d >= d.zero() ? d : -d;
}

}  // namespace etl::chrono

namespace etl
{
/**
 * @brief Exposes the type named type, which is the common type of two
 * etl::chrono::durations, whose period is the greatest common divisor of
 * Period1 and Period2.
 *
 * @details The period of the resulting duration can be computed by forming a
 * ratio of the greatest common divisor of Period1::num and Period2::num and the
 * least common multiple of Period1::den and Period2::den.
 */
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
struct common_type<etl::chrono::duration<Rep1, Period1>,
                   etl::chrono::duration<Rep2, Period2>>
{
  using type
    = etl::chrono::duration<typename etl::common_type<Rep1, Rep2>::type,
                            ratio<etl::gcd(Period1::num, Period2::num),
                                  etl::lcm(Period1::den, Period2::den)>>;
};

inline namespace literals
{
inline namespace chrono_literals
{
/**
 * @brief Forms a etl::chrono::duration literal representing hours. Integer
 * literal, returns exactly etl::chrono::hours(hrs).
 */
constexpr auto operator""_h(unsigned long long h) -> etl::chrono::hours
{
  return etl::chrono::hours(static_cast<etl::chrono::hours::rep>(h));
}

/**
 * @brief Forms a etl::chrono::duration literal representing hours.
 * Floating-point literal, returns a floating-point duration equivalent to
 * etl::chrono::hours.
 */
constexpr auto operator""_h(long double h)
  -> etl::chrono::duration<long double, ratio<3600, 1>>
{
  return etl::chrono::duration<long double, etl::ratio<3600, 1>>(h);
}

/**
 * @brief Forms a etl::chrono::duration literal representing minutes. Integer
 * literal, returns exactly etl::chrono::minutes(mins).
 */
constexpr auto operator""_min(unsigned long long m) -> etl::chrono::minutes
{
  return etl::chrono::minutes(static_cast<etl::chrono::minutes::rep>(m));
}

/**
 * @brief Forms a etl::chrono::duration literal representing minutes.
 * Floating-point literal, returns a floating-point duration equivalent to
 * etl::chrono::minutes.
 */
constexpr auto operator""_min(long double m)
  -> etl::chrono::duration<long double, etl::ratio<60, 1>>
{
  return etl::chrono::duration<long double, ratio<60, 1>>(m);
}

/**
 * @brief Forms a etl::chrono::duration literal representing seconds. Integer
 * literal, returns exactly etl::chrono::seconds(mins).
 */
constexpr auto operator""_s(unsigned long long m) -> etl::chrono::seconds
{
  return etl::chrono::seconds(static_cast<etl::chrono::seconds::rep>(m));
}

/**
 * @brief Forms a etl::chrono::duration literal representing seconds.
 * Floating-point literal, returns a floating-point duration equivalent to
 * etl::chrono::seconds.
 */
constexpr auto operator""_s(long double m) -> etl::chrono::duration<long double>
{
  return etl::chrono::duration<long double>(m);
}

/**
 * @brief Forms a etl::chrono::duration literal representing milliseconds.
 * Integer literal, returns exactly etl::chrono::milliseconds(mins).
 */
constexpr auto operator""_ms(unsigned long long m) -> etl::chrono::milliseconds
{
  return etl::chrono::milliseconds(
    static_cast<etl::chrono::milliseconds::rep>(m));
}

/**
 * @brief Forms a etl::chrono::duration literal representing milliseconds.
 * Floating-point literal, returns a floating-point duration equivalent to
 * etl::chrono::milliseconds.
 */
constexpr auto operator""_ms(long double m)
  -> etl::chrono::duration<long double, etl::milli>
{
  return etl::chrono::duration<long double, etl::milli>(m);
}

/**
 * @brief Forms a etl::chrono::duration literal representing microseconds.
 * Integer literal, returns exactly etl::chrono::microseconds(mins).
 */
constexpr auto operator""_us(unsigned long long m) -> etl::chrono::microseconds
{
  return etl::chrono::microseconds(
    static_cast<etl::chrono::microseconds::rep>(m));
}

/**
 * @brief Forms a etl::chrono::duration literal representing microseconds.
 * Floating-point literal, returns a floating-point duration equivalent to
 * etl::chrono::microseconds.
 */
constexpr auto operator""_us(long double m)
  -> etl::chrono::duration<long double, etl::micro>
{
  return etl::chrono::duration<long double, etl::micro>(m);
}

/**
 * @brief Forms a etl::chrono::duration literal representing nanoseconds.
 * Integer literal, returns exactly etl::chrono::nanoseconds(mins).
 */
constexpr auto operator""_ns(unsigned long long m) -> etl::chrono::nanoseconds
{
  return etl::chrono::nanoseconds(
    static_cast<etl::chrono::nanoseconds::rep>(m));
}

/**
 * @brief Forms a etl::chrono::duration literal representing nanoseconds.
 * Floating-point literal, returns a floating-point duration equivalent to
 * etl::chrono::nanoseconds.
 */
constexpr auto operator""_ns(long double m)
  -> etl::chrono::duration<long double, etl::nano>
{
  return etl::chrono::duration<long double, etl::nano>(m);
}

}  // namespace chrono_literals
}  // namespace literals
namespace chrono
{
using namespace ::etl::literals::chrono_literals;
}
}  // namespace etl
#endif  // TAETL_CHRONO_HPP