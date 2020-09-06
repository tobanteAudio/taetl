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

#include "etl/definitions.hpp"
#include "etl/ratio.hpp"

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
template <class Rep>
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
 * @brief Class template etl::chrono::duration represents a time interval.
 *
 * @details It consists of a count of ticks of type Rep and a tick period, where
 * the tick period is a compile-time rational constant representing the number
 * of seconds from one tick to the next. The only data stored in a duration is a
 * tick count of type Rep. If Rep is floating point, then the duration can
 * represent fractions of ticks. Period is included as part of the duration's
 * type, and is only used when converting between different durations.
 */
template <class Rep, class Period = etl::ratio<1>>
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
    constexpr duration() = default;

    /**
     * @brief Constructs a new duration from one of several optional data
     * sources. The copy constructor is defaulted (makes a bitwise copy of the
     * tick count).
     */
    duration(const duration&) = default;

    /**
     * @brief Constructs a new duration from one of several optional data
     * sources.
     */
    template <class Rep2>
    constexpr explicit duration(const Rep2& r) : data_(r)
    {
    }

    // /**
    //  * @brief Constructs a new duration from one of several optional data
    //  * sources.
    //  */
    // template <class Rep2, class Period2>
    // constexpr duration(const duration<Rep2, Period2>& d)

    /**
     * @brief Assigns the contents of one duration to another.
     */
    auto operator=(const duration& other) -> duration& = default;

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

private:
    rep data_ {};
};

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
using days = duration<etl::int32_t, etl::ratio<86400>>;

/**
 * @brief Signed integer type of at least 22 bits.
 */
using weeks = duration<etl::int32_t, etl::ratio<604800>>;

/**
 * @brief Signed integer type of at least 20 bits.
 */
using months = duration<etl::int32_t, etl::ratio<2629746>>;

/**
 * @brief Signed integer type of at least 17 bits.
 */
using years = duration<etl::int32_t, etl::ratio<31556952>>;

}  // namespace etl::chrono

namespace etl
{
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