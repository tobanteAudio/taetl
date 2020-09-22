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

#ifndef TAETL_VARIANT_HPP
#define TAETL_VARIANT_HPP

#include "etl/definitions.hpp"
#include "etl/new.hpp"
#include "etl/type_traits.hpp"
#include "etl/utility.hpp"

namespace etl
{
struct monostate
{
};

namespace detail
{
using union_index_type = size_t;

template <typename...>
struct variant_data;

template <typename Head_>
struct variant_data<Head_>
{
    using Head = Head_;
    alignas(Head) unsigned char data[sizeof(Head)];

    template <typename T>
    void construct(T&& head_init, union_index_type& union_index)
    {
        static_assert(etl::is_same<T, Head>::value,
                      "Tried to access non-existent type in union");
        new (data) Head(etl::forward<T>(head_init));
        union_index = 0;
    }

    void destruct(union_index_type)
    {
        static_cast<Head*>(static_cast<void*>(data))->~Head();
    }
};

template <typename Head_, typename... Tail>
struct variant_data<Head_, Tail...>
{
    using Head = Head_;
    union
    {
        alignas(Head) unsigned char data[sizeof(Head)];
        variant_data<Tail...> tail;
    };

    //

    void construct(const Head& head_init, union_index_type& union_index)
    {
        new (data) Head(head_init);
        union_index = 0;
    }

    void construct(Head& head_init, union_index_type& union_index)
    {
        const auto& head_cref = head_init;
        construct(head_cref, union_index);
    }

    void construct(Head&& head_init, union_index_type& union_index)
    {
        using etl::move;
        new (data) Head(move(head_init));
        union_index = 0;
    }

    template <typename T>
    void construct(T&& t, union_index_type& union_index)
    {
        tail.construct(etl::forward<T>(t), union_index);
        ++union_index;
    }

    //

    void destruct(union_index_type union_index)
    {
        if (union_index == 0) { static_cast<Head*>(static_cast<void*>(data))->~Head(); }
        else
        {
            tail.destruct(union_index - 1);
        }
    }
};
template <typename...>
struct variant_data_get;

template <typename Head, typename... Tail>
struct variant_data_get<Head, variant_data<Head, Tail...>>
{
    static auto get(variant_data<Head, Tail...>& vu) -> Head&
    {
        return *static_cast<Head*>(static_cast<void*>(vu.data));
    }

    static auto get(variant_data<Head, Tail...> const& vu) -> Head const&
    {
        return *static_cast<Head const*>(static_cast<void const*>(vu.data));
    }

    static constexpr const union_index_type index = 0;
};

template <typename Target, typename Head, typename... Tail>
struct variant_data_get<Target, variant_data<Head, Tail...>>
{
    static auto get(variant_data<Head, Tail...>& vu) -> Target&
    {
        return variant_data_get<Target, variant_data<Tail...>>::get(vu.tail);
    }

    static auto get(variant_data<Head, Tail...> const& vu) -> Target const&
    {
        return variant_data_get<Target, variant_data<Tail...>>::get(vu.tail);
    }

    static constexpr const union_index_type index
        = variant_data_get<Target, variant_data<Tail...>>::index + 1;
};

}  // namespace detail

/**
 * @brief This is a special value equal to the largest value representable by the type
 * etl::size_t, used as the return value of index() when valueless_by_exception() is true.
 */
inline constexpr auto variant_npos = static_cast<etl::size_t>(-1);

/**
 * @brief The class template etl::variant represents a type-safe union. An instance of
 * etl::variant at any given time either holds a value of one of its alternative types.
 */
template <typename... Types>
class variant
{
public:
    /**
     * @brief Converting constructor.
     *
     * @details Constructs a variant holding the alternative type T.
     */
    template <typename T>
    variant(T&& t)
    {
        data_.construct(etl::forward<T>(t), union_index_);
    }

    /**
     * @brief If valueless_by_exception is true, does nothing. Otherwise, destroys the
     * currently contained value.
     *
     * @todo This destructor is trivial if etl::is_trivially_destructible_v<T_i> is true
     * for all T_i in Types...
     */
    ~variant()
    {
        if (!valueless_by_exception()) { data_.destruct(union_index_); }
    }

    /**
     * @brief Returns the zero-based index of the alternative that is currently held by
     * the variant. If the variant is valueless_by_exception, returns variant_npos.
     */
    [[nodiscard]] constexpr auto index() const noexcept -> etl::size_t
    {
        return valueless_by_exception() ? variant_npos : union_index_;
    }

    /**
     * @brief Returns false if and only if the variant holds a value. Currently always
     * returns false, since there is no default constructor.
     */
    [[nodiscard]] constexpr auto valueless_by_exception() const noexcept -> bool
    {
        return false;
    }

    /**
     * @todo Remove & replace with friendship for etl::get_if.
     */
    auto data() const noexcept { return &data_; }
    auto data() noexcept { return &data_; }

private:
    detail::variant_data<Types...> data_;
    detail::union_index_type union_index_;
};

/**
 * @brief Checks if the variant v holds the alternative T. The call is ill-formed if T
 * does not appear exactly once in Types...
 */
template <class T, class... Types>
constexpr auto holds_alternative(etl::variant<Types...> const& v) noexcept -> bool
{
    using storage_t = detail::variant_data<Types...>;
    return detail::variant_data_get<T, storage_t>::index == v.index();
}

/**
 * @brief Type-based non-throwing accessor: Equivalent to (1) with I being the
 * zero-based index of T in Types.... The call is ill-formed if T is not a unique
 * element of Types....
 */
template <class T, class... Types>
constexpr auto get_if(etl::variant<Types...>* pv) noexcept -> etl::add_pointer_t<T>
{
    using storage_t = detail::variant_data<Types...>;
    if (holds_alternative<T>(*pv))
    { return &detail::variant_data_get<T, storage_t>::get(*pv->data()); }

    return nullptr;
}

/**
 * @brief Type-based non-throwing accessor: Equivalent to (1) with I being the zero-based
 * index of T in Types.... The call is ill-formed if T is not a unique element of
 * Types....
 */
template <class T, class... Types>
constexpr auto get_if(etl::variant<Types...> const* pv) noexcept
    -> etl::add_pointer_t<const T>
{
    using storage_t = detail::variant_data<Types...>;
    if (holds_alternative<T>(*pv))
    { return &detail::variant_data_get<T, storage_t>::get(*pv->data()); }

    return nullptr;
}

}  // namespace etl

#endif  // TAETL_VARIANT_HPP