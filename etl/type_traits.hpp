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

/**
 * @example type_traits.cpp
 */

#ifndef TAETL_TYPETRAITS_HPP
#define TAETL_TYPETRAITS_HPP

#include "etl/detail/cstddef_internal.hpp"  // for size_t, max_align_t, nullptr_t
#include "etl/detail/intrinsics.hpp"        // for TAETL_IS_CLASS, TAETL_IS_ENUM
#include "etl/detail/sfinae.hpp"            // for enable_if
#include "etl/detail/type_traits_decl.hpp"  // for is_fundamental

namespace etl
{
// integral_constant
template <typename Type, Type val>
struct integral_constant
{
  static constexpr Type value = val;
  using value_type            = Type;
  using type                  = integral_constant<Type, val>;
  constexpr operator value_type() const noexcept { return value; }
  constexpr auto operator()() const noexcept -> value_type { return value; }
};

template <bool B>
using bool_constant = integral_constant<bool, B>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename...>
using void_t = void;

template <typename T>
struct type_identity
{
  using type = T;
};

/**
 * @brief Provides member typedef type, which is defined as T if B is true at
 * compile time, or as F if B is false.
 */
template <bool B, typename T, typename F>
struct conditional
{
  using type = T;
};

template <typename T, typename F>
struct conditional<false, T, F>
{
  using type = F;
};

template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

/**
 * @brief Forms the logical conjunction of the type traits B..., effectively
 * performing a logical AND on the sequence of traits.
 */
template <typename...>
struct conjunction : etl::true_type
{
};

template <typename B1>
struct conjunction<B1> : B1
{
};

template <typename B1, typename... Bn>
struct conjunction<B1, Bn...>
    : etl::conditional_t<bool(B1::value), conjunction<Bn...>, B1>
{
};

template <typename... B>
inline constexpr bool conjunction_v = conjunction<B...>::value;

/**
 * @brief Forms the logical disjunction of the type traits B..., effectively
 * performing a logical OR on the sequence of traits.
 */
template <typename...>
struct disjunction : etl::false_type
{
};
template <typename B1>
struct disjunction<B1> : B1
{
};
template <typename B1, typename... Bn>
struct disjunction<B1, Bn...>
    : etl::conditional_t<bool(B1::value), B1, disjunction<Bn...>>
{
};

template <typename... B>
inline constexpr bool disjunction_v = disjunction<B...>::value;

/**
 * @brief Forms the logical negation of the type trait B.
 */
template <typename B>
struct negation : etl::bool_constant<!bool(B::value)>
{
};

template <typename B>
inline constexpr bool negation_v = negation<B>::value;

namespace detail
{
template <typename T>
auto try_add_lvalue_reference(int) -> ::etl::type_identity<T&>;
template <typename T>
auto try_add_lvalue_reference(...) -> ::etl::type_identity<T>;

template <typename T>
auto try_add_rvalue_reference(int) -> ::etl::type_identity<T&&>;
template <typename T>
auto try_add_rvalue_reference(...) -> ::etl::type_identity<T>;

}  // namespace detail

template <typename T>
struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0))
{
};

template <typename T>
struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0))
{
};

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

template <typename T>
auto declval() noexcept -> typename etl::add_rvalue_reference<T>::type;

template <typename T, unsigned N = 0>
struct extent : etl::integral_constant<etl::size_t, 0>
{
};

template <typename T>
struct extent<T[], 0> : etl::integral_constant<etl::size_t, 0>
{
};

template <typename T, unsigned N>
struct extent<T[], N> : etl::extent<T, N - 1>
{
};

template <typename T, etl::size_t I>
struct extent<T[I], 0> : etl::integral_constant<etl::size_t, I>
{
};

template <typename T, etl::size_t I, unsigned N>
struct extent<T[I], N> : etl::extent<T, N - 1>
{
};

/**
 * @brief If T is an array of some type X, provides the member typedef type
 * equal to X, otherwise type is T. Note that if T is a multidimensional array,
 * only the first dimension is removed. The behavior of a program that adds
 * specializations for remove_extent is undefined.
 */
template <typename T>
struct remove_extent
{
  using type = T;
};

template <typename T>
struct remove_extent<T[]>
{
  using type = T;
};

template <typename T, etl::size_t N>
struct remove_extent<T[N]>
{
  using type = T;
};

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

/**
 * @brief If T is a multidimensional array of some type X, provides the member
 * typedef type equal to X, otherwise type is T. The behavior of a program that
 * adds specializations for remove_all_extents is undefined.
 */
template <typename T>
struct remove_all_extents
{
  using type = T;
};

/**
 * @brief If T is a multidimensional array of some type X, provides the member
 * typedef type equal to X, otherwise type is T. The behavior of a program that
 * adds specializations for remove_all_extents is undefined.
 */
template <typename T>
struct remove_all_extents<T[]>
{
  using type = typename remove_all_extents<T>::type;
};

/**
 * @brief If T is a multidimensional array of some type X, provides the member
 * typedef type equal to X, otherwise type is T. The behavior of a program that
 * adds specializations for remove_all_extents is undefined.
 */
template <typename T, etl::size_t N>
struct remove_all_extents<T[N]>
{
  using type = typename remove_all_extents<T>::type;
};

/**
 * @brief If T is a multidimensional array of some type X, provides the member
 * typedef type equal to X, otherwise type is T. The behavior of a program that
 * adds specializations for remove_all_extents is undefined.
 */
template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

/**
 * @brief Provides the member typedef type which is the same as T, except that
 * its topmost cv-qualifiers are removed. Removes the topmost const.
 * @details The behavior of a program that adds specializations for any of the
 * templates described on this page is undefined.
 */
template <typename Type>
struct remove_const
{
  using type = Type;
};

template <typename Type>
struct remove_const<Type const>
{
  using type = Type;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

/**
 * @brief Provides the member typedef type which is the same as T, except that
 * its topmost cv-qualifiers are removed. Removes the topmost volatile.
 * @details The behavior of a program that adds specializations for any of the
 * templates described on this page is undefined.
 */

template <typename Type>
struct remove_volatile
{
  using type = Type;
};

template <typename Type>
struct remove_volatile<Type volatile>
{
  using type = Type;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

/**
 * @brief Provides the member typedef type which is the same as T, except that
 * its topmost cv-qualifiers are removed. Removes the topmost const, or the
 * topmost volatile, or both, if present.
 * @details The behavior of a program that adds specializations for any of the
 * templates described on this page is undefined.
 */
template <typename Type>
struct remove_cv
{
  using type = remove_const_t<remove_volatile_t<Type>>;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
struct remove_reference
{
  using type = T;
};

template <typename T>
struct remove_reference<T&>
{
  using type = T;
};

template <typename T>
struct remove_reference<T&&>
{
  using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

/**
 * @brief If the type T is a reference type, provides the member typedef type
 * which is the type referred to by T with its topmost cv-qualifiers removed.
 * Otherwise type is T with its topmost cv-qualifiers removed.
 *
 * @details The behavior of a program that adds specializations for remove_cvref
 * is undefined.
 */
template <typename T>
struct remove_cvref
{
  using type = etl::remove_cv_t<etl::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

/**
 * @brief Provides the member typedef type which is the type pointed to by T,
 * or, if T is not a pointer, then type is the same as T. The behavior of a
 * program that adds specializations for remove_pointer is undefined.
 */
template <typename T>
struct remove_pointer
{
  using type = T;
};
template <typename T>
struct remove_pointer<T*>
{
  using type = T;
};
template <typename T>
struct remove_pointer<T* const>
{
  using type = T;
};
template <typename T>
struct remove_pointer<T* volatile>
{
  using type = T;
};
template <typename T>
struct remove_pointer<T* const volatile>
{
  using type = T;
};

template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

namespace detail
{
template <typename T>
auto try_add_pointer(int)
  -> ::etl::type_identity<::etl::remove_reference_t<T>*>;
template <typename T>
auto try_add_pointer(...) -> ::etl::type_identity<T>;

}  // namespace detail

/**
 * @brief If T is a reference type, then provides the member typedef type which
 * is a pointer to the referred type. Otherwise, if T names an object type, a
 * function type that is not cv- or ref-qualified, or a (possibly cv-qualified)
 * void type, provides the member typedef type which is the type T*. Otherwise
 * (if T is a cv- or ref-qualified function type), provides the member typedef
 * type which is the type T. The behavior of a program that adds specializations
 * for add_pointer is undefined.
 */
template <typename T>
struct add_pointer : decltype(detail::try_add_pointer<T>(0))
{
};

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

/**
 * @brief Provides the member typedef type which is the same as T, except it has
 * a cv-qualifier added (unless T is a function, a reference, or already has
 * this cv-qualifier). Adds both const and volatile.
 */
template <typename T>
struct add_cv
{
  using type = T const volatile;
};

template <typename T>
using add_cv_t = typename add_cv<T>::type;

/**
 * @brief Provides the member typedef type which is the same as T, except it has
 * a cv-qualifier added (unless T is a function, a reference, or already has
 * this cv-qualifier). Adds const.
 */
template <typename T>
struct add_const
{
  using type = T const;
};

template <typename T>
using add_const_t = typename add_const<T>::type;

/**
 * @brief Provides the member typedef type which is the same as T, except it has
 * a cv-qualifier added (unless T is a function, a reference, or already has
 * this cv-qualifier). Adds volatile.
 */
template <typename T>
struct add_volatile
{
  using type = T volatile;
};

template <typename T>
using add_volatile_t = typename add_volatile<T>::type;

/**
 * @brief If T and U name the same type (taking into account const/volatile
 * qualifications), provides the member constant value equal to true. Otherwise
 * value is false.
 */
template <typename T, typename U>
struct is_same : false_type
{
};

template <typename T>
struct is_same<T, T> : true_type
{
};

template <typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;

/**
 * @brief Define a member typedef only if a boolean constant is true.
 */
template <typename T>
struct is_void : is_same<void, typename remove_cv<T>::type>
{
};

template <typename T>
inline constexpr bool is_void_v = is_void<T>::value;

namespace detail
{
template <typename>
struct is_integral_helper : public ::etl::false_type
{
};

template <>
struct is_integral_helper<bool> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<char> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<signed char> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<unsigned char> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<char16_t> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<char32_t> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<short> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<unsigned short> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<int> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<unsigned int> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<long> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<unsigned long> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<long long> : public ::etl::true_type
{
};

template <>
struct is_integral_helper<unsigned long long> : public ::etl::true_type
{
};
}  // namespace detail

// is_integral
template <typename Type>
struct is_integral
    : public detail::is_integral_helper<typename remove_cv<Type>::type>::type
{
};

template <typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

namespace detail
{
template <typename>
struct make_signed_helper;

template <>
struct make_signed_helper<signed char>
{
  using type = signed char;
};

template <>
struct make_signed_helper<signed short>
{
  using type = signed short;
};

template <>
struct make_signed_helper<signed int>
{
  using type = signed int;
};

template <>
struct make_signed_helper<signed long>
{
  using type = signed long;
};

template <>
struct make_signed_helper<signed long long>
{
  using type = signed long long;
};

template <>
struct make_signed_helper<unsigned char>
{
  using type = signed char;
};

template <>
struct make_signed_helper<unsigned short>
{
  using type = signed short;
};

template <>
struct make_signed_helper<unsigned int>
{
  using type = signed int;
};

template <>
struct make_signed_helper<unsigned long>
{
  using type = signed long;
};

template <>
struct make_signed_helper<unsigned long long>
{
  using type = signed long long;
};

}  // namespace detail

/**
 * @brief If T is an integral (except bool) or enumeration type, provides the
 * member typedef type which is the unsigned integer type corresponding to T,
 * with the same cv-qualifiers. If T is signed or unsigned char, short, int,
 * long, long long; the unsigned type from this list corresponding to T is
 * provided. The behavior of a program that adds specializations for
 * make_signed is undefined.
 */
template <typename Type>
struct make_signed : detail::make_signed_helper<Type>
{
};

template <typename T>
using make_signed_t = typename make_signed<T>::type;

namespace detail
{
template <typename>
struct make_unsigned_helper;

template <>
struct make_unsigned_helper<signed char>
{
  using type = unsigned char;
};

template <>
struct make_unsigned_helper<signed short>
{
  using type = unsigned short;
};

template <>
struct make_unsigned_helper<signed int>
{
  using type = unsigned int;
};

template <>
struct make_unsigned_helper<signed long>
{
  using type = unsigned long;
};

template <>
struct make_unsigned_helper<signed long long>
{
  using type = unsigned long long;
};

template <>
struct make_unsigned_helper<unsigned char>
{
  using type = unsigned char;
};

template <>
struct make_unsigned_helper<unsigned short>
{
  using type = unsigned short;
};

template <>
struct make_unsigned_helper<unsigned int>
{
  using type = unsigned int;
};

template <>
struct make_unsigned_helper<unsigned long>
{
  using type = unsigned long;
};

template <>
struct make_unsigned_helper<unsigned long long>
{
  using type = unsigned long long;
};

}  // namespace detail

/**
 * @brief If T is an integral (except bool) or enumeration type, provides the
 * member typedef type which is the unsigned integer type corresponding to T,
 * with the same cv-qualifiers. If T is signed or unsigned char, short, int,
 * long, long long; the unsigned type from this list corresponding to T is
 * provided. The behavior of a program that adds specializations for
 * make_unsigned is undefined.
 */
template <typename Type>
struct make_unsigned : detail::make_unsigned_helper<Type>
{
};

template <typename T>
using make_unsigned_t = typename make_unsigned<T>::type;

/**
 * @brief Checks whether T is a floating-point type. Provides the member
 * constant value which is equal to true, if T is the type float, double, long
 * double, including any cv-qualified variants. Otherwise, value is equal to
 * false.
 *
 * @details The behavior of a program that adds specializations for
 * is_floating_point or is_floating_point_v is undefined.
 */
template <typename T>
struct is_floating_point
    : etl::bool_constant<
        etl::is_same<float, etl::remove_cv_t<T>>::value
        || etl::is_same<double, etl::remove_cv_t<T>>::value
        || etl::is_same<long double, etl::remove_cv_t<T>>::value>
{
};

template <typename T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

/**
 * @brief If T is a const-qualified type (that is, const, or const volatile),
 * provides the member constant value equal to true. For any other type, value
 * is false.
 */
template <typename T>
struct is_const : etl::false_type
{
};

template <typename T>
struct is_const<const T> : etl::true_type
{
};

template <typename T>
inline constexpr bool is_const_v = is_const<T>::value;

template <typename T>
struct is_volatile : etl::false_type
{
};
template <typename T>
struct is_volatile<volatile T> : etl::true_type
{
};

template <typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;

namespace detail
{
template <typename T>
struct is_empty_test_struct_1 : public T
{
  char _dummy_data_;
};

struct is_empty_test_struct_2
{
  char _dummy_data_;
};

template <typename T, bool = ::etl::is_class<T>::value>
struct is_empty_helper
    : public ::etl::bool_constant<sizeof(is_empty_test_struct_1<T>)
                                  == sizeof(is_empty_test_struct_2)>
{
};

template <typename T>
struct is_empty_helper<T, false> : public ::etl::false_type
{
};
}  // namespace detail

/**
 * @brief f T is an empty type (that is, a non-union class type with no
 * non-static data members other than bit-fields of size 0, no virtual
 * functions, no virtual base classes, and no non-empty base classes), provides
 * the member constant value equal to true. For any other type, value is false.
 */
template <typename T>
struct is_empty : public detail::is_empty_helper<T>
{
};

template <typename T>
inline constexpr bool is_empty_v = is_empty<T>::value;

/**
 * @brief is_polymorphic
 */
template <typename T>
struct is_polymorphic : public bool_constant<TAETL_IS_POLYMORPHIC(T)>
{
};

template <typename T>
inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;

/**
 * @brief If T is a final class (that is, a class declared with the final
 * specifier), provides the member constant value equal true. For any other
 * type, value is false. If T is a class type, T shall be a complete type;
 * otherwise, the behavior is undefined.
 */
template <typename T>
struct is_final : public bool_constant<TAETL_IS_FINAL(T)>
{
};

template <typename T>
inline constexpr bool is_final_v = is_final<T>::value;

/**
 * @brief is_abstract
 */
template <typename T>
struct is_abstract : public bool_constant<TAETL_IS_ABSTRACT(T)>
{
};

template <typename T>
inline constexpr bool is_abstract_v = is_abstract<T>::value;

/**
 * @brief is_aggregate
 */
template <typename T>
struct is_aggregate : bool_constant<TAETL_IS_AGGREGATE(remove_cv_t<T>)>
{
};

template <typename T>
inline constexpr bool is_aggregate_v = is_aggregate<T>::value;

/**
 * @brief If T is a reference type (lvalue reference or rvalue reference),
 * provides the member constant value equal true. For any other type, value is
 * false. The behavior of a program that adds specializations for is_reference
 * or is_reference_v is undefined.
 */
template <typename T>
struct is_reference : etl::false_type
{
};
template <typename T>
struct is_reference<T&> : etl::true_type
{
};
template <typename T>
struct is_reference<T&&> : etl::true_type
{
};
template <typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;

template <typename T>
struct is_null_pointer : is_same<nullptr_t, remove_cv_t<T>>
{
};

template <typename T>
inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

/**
 * @brief Checks whether T is an array type. Provides the member constant value
 * which is equal to true, if T is an array type. Otherwise, value is equal to
 * false.
 * @details The behavior of a program that adds specializations for is_array or
 * is_array_v is undefined.
 */
template <typename T>
struct is_array : false_type
{
};

template <typename T>
struct is_array<T[]> : true_type
{
};

template <typename T, size_t N>
struct is_array<T[N]> : true_type
{
};

template <typename T>
inline constexpr bool is_array_v = is_array<T>::value;

/**
 * @brief Checks whether T is a function type. Types like etl::function,
 * lambdas, classes with overloaded operator() and pointers to functions don't
 * count as function types. Provides the member constant value which is equal to
 * true, if T is a function type. Otherwise, value is equal to false.
 *
 * @details The behavior of a program that adds specializations for is_function
 * or is_function_v is undefined.
 */
template <typename T>
struct is_function
    : etl::bool_constant<!etl::is_const_v<T const> && !etl::is_reference_v<T>>
{
};

template <typename T>
inline constexpr bool is_function_v = is_function<T>::value;

namespace detail
{
template <typename T>
struct is_pointer_helper : ::etl::false_type
{
};
template <typename T>
struct is_pointer_helper<T*> : ::etl::true_type
{
};
}  // namespace detail

/**
 * @brief Checks whether T is a pointer to object or a pointer to function (but
 * not a pointer to member/member function). Provides the member constant value
 * which is equal to true, if T is a object/function pointer type. Otherwise,
 * value is equal to false.
 *
 * @details The behavior of a program that adds specializations for is_pointer
 * or is_pointer_v is undefined.
 */
template <typename T>
struct is_pointer : detail::is_pointer_helper<typename remove_cv<T>::type>
{
};

template <typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

/**
 * @brief Checks whether T is a lvalue reference type. Provides the member
 * constant value which is equal to true, if T is a lvalue reference type.
 * Otherwise, value is equal to false.
 */
template <typename T>
struct is_lvalue_reference : etl::false_type
{
};
template <typename T>
struct is_lvalue_reference<T&> : etl::true_type
{
};

template <typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

/**
 * @brief Checks whether T is a rvalue reference type. Provides the member
 * constant value which is equal to true, if T is a rvalue reference type.
 * Otherwise, value is equal to false.
 */
template <typename T>
struct is_rvalue_reference : etl::false_type
{
};
template <typename T>
struct is_rvalue_reference<T&&> : etl::true_type
{
};

template <typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <typename T>
struct is_class : etl::bool_constant<TAETL_IS_CLASS(T)>
{
};

template <typename T>
inline constexpr bool is_class_v = is_class<T>::value;

template <typename T>
struct is_enum : etl::bool_constant<TAETL_IS_ENUM(T)>
{
};

template <typename T>
inline constexpr bool is_enum_v = is_enum<T>::value;

template <typename T>
struct is_union : etl::bool_constant<TAETL_IS_UNION(T)>
{
};

template <typename T>
inline constexpr bool is_union_v = is_union<T>::value;

namespace detail
{
template <typename T>
struct is_member_pointer_helper : ::etl::false_type
{
};

template <typename T, typename U>
struct is_member_pointer_helper<T U::*> : ::etl::true_type
{
};
}  // namespace detail

/**
 * @brief If T is pointer to non-static member object or a pointer to non-static
 * member function, provides the member constant value equal true. For any other
 * type, value is false. The behavior of a program that adds specializations for
 * is_member_pointer or is_member_pointer_v (since C++17) is undefined.
 */
template <typename T>
struct is_member_pointer : detail::is_member_pointer_helper<etl::remove_cv_t<T>>
{
};

template <typename T>
inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

namespace detail
{
template <typename T>
struct is_member_function_pointer_helper : ::etl::false_type
{
};

template <typename T, typename U>
struct is_member_function_pointer_helper<T U::*> : ::etl::is_function<T>
{
};

}  // namespace detail

/**
 * @brief Checks whether T is a non-static member function pointer. Provides the
 * member constant value which is equal to true, if T is a non-static member
 * function pointer type. Otherwise, value is equal to false.
 */
template <typename T>
struct is_member_function_pointer
    : detail::is_member_function_pointer_helper<etl::remove_cv_t<T>>
{
};

template <typename T>
inline constexpr bool is_member_function_pointer_v
  = is_member_function_pointer<T>::value;

/**
 * @brief Checks whether T is a non-static member object pointer. Provides the
 * member constant value which is equal to true, if T is a non-static member
 * object pointer type. Otherwise, value is equal to false.
 */
template <typename T>
struct is_member_object_pointer
    : etl::bool_constant<
        etl::is_member_pointer_v<T> && !etl::is_member_function_pointer_v<T>>
{
};

template <typename T>
inline constexpr bool is_member_object_pointer_v
  = is_member_object_pointer<T>::value;

/**
 * @brief If T is an arithmetic type (that is, an integral type or a
 * floating-point type) or a cv-qualified version thereof, provides the member
 * constant value equal true. For any other type, value is false. The behavior
 * of a program that adds specializations for is_arithmetic or is_arithmetic_v
 * (since C++17) is undefined.
 */
template <typename T>
struct is_arithmetic
    : etl::bool_constant<etl::is_integral_v<T> || etl::is_floating_point_v<T>>
{
};

template <typename T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

/**
 * @brief If T is a fundamental type (that is, arithmetic type, void, or
 * nullptr_t), provides the member constant value equal true. For any other
 * type, value is false.
 */
template <typename T>
struct is_fundamental
    : etl::bool_constant<etl::is_arithmetic_v<
                           T> || etl::is_void_v<T> || etl::is_null_pointer_v<T>>
{
};

template <typename T>
inline constexpr bool is_fundamental_v = is_fundamental<T>::value;

/**
 * @brief If T is a scalar type (that is a possibly cv-qualified arithmetic,
 * pointer, pointer to member, enumeration, or etl::nullptr_t type), provides
 * the member constant value equal true. For any other type, value is false.
 */
template <typename T>
struct is_scalar
    : etl::bool_constant<
        etl::is_arithmetic_v<
          T> || etl::is_enum_v<T> || etl::is_pointer_v<T> || etl::is_member_pointer_v<T> || etl::is_null_pointer_v<T>>
{
};

template <typename T>
inline constexpr bool is_scalar_v = is_scalar<T>::value;

/**
 * @brief If T is an object type (that is any possibly cv-qualified type other
 * than function, reference, or void types), provides the member constant value
 * equal true. For any other type, value is false.
 */
template <typename T>
struct is_object
    : etl::bool_constant<
        etl::is_scalar_v<
          T> || etl::is_array_v<T> || etl::is_union_v<T> || etl::is_class_v<T>>
{
};

template <typename T>
inline constexpr bool is_object_v = is_object<T>::value;

/**
 * @brief If T is a compound type (that is, array, function, object pointer,
 * function pointer, member object pointer, member function pointer, reference,
 * class, union, or enumeration, including any cv-qualified variants), provides
 * the member constant value equal true. For any other type, value is false.
 */
template <typename T>
struct is_compound : etl::bool_constant<!etl::is_fundamental_v<T>>
{
};

template <typename T>
inline constexpr bool is_compound_v = is_compound<T>::value;

/**
 * @brief Checks whether T is an array type of known bound. Provides the member
 * constant value which is equal to true, if T is an array type of known bound.
 * Otherwise, value is equal to false.
 */
template <typename T>
struct is_bounded_array : etl::false_type
{
};

template <typename T, etl::size_t N>
struct is_bounded_array<T[N]> : etl::true_type
{
};

template <typename T>
inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

/**
 * @brief Checks whether T is an array type of unknown bound. Provides the
 * member constant value which is equal to true, if T is an array type of
 * unknown bound. Otherwise, value is equal to false.
 */
template <typename T>
struct is_unbounded_array : etl::false_type
{
};

template <typename T>
struct is_unbounded_array<T[]> : etl::true_type
{
};

template <typename T>
inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

namespace detail
{
template <typename, typename T, typename... Args>
struct is_constructible_helper : ::etl::false_type
{
};

template <typename T, typename... Args>
struct is_constructible_helper<
  ::etl::void_t<decltype(T(::etl::declval<Args>()...))>, T, Args...>
    : ::etl::true_type
{
};
}  // namespace detail

template <typename T, typename... Args>
using is_constructible = detail::is_constructible_helper<void_t<>, T, Args...>;

template <typename T, typename... Args>
inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

/**
 * @brief The variable definition does not call any operation that is not
 * trivial. For the purposes of this check, the call to etl::declval is
 * considered trivial.
 */
template <typename T, typename... Args>
struct is_trivially_constructible
    : etl::bool_constant<TAETL_IS_TRIVIAL_CONSTRUCTIBLE(T)>
{
};

template <typename T, typename... Args>
inline constexpr bool is_trivially_constructible_v
  = is_trivially_constructible<T, Args...>::value;

/**
 * @brief The variable definition does not call any operation that is not
 * trivial. For the purposes of this check, the call to etl::declval is
 * considered trivial.
 */
template <typename T, typename... Args>
struct is_nothrow_constructible
    : etl::bool_constant<TAETL_IS_NOTHROW_CONSTRUCTIBLE(T)>
{
};

template <typename T, typename... Args>
inline constexpr bool is_nothrow_constructible_v
  = is_nothrow_constructible<T, Args...>::value;

/**
 * @brief If etl::is_constructible<T>::value is true, provides the member
 * constant value equal to true, otherwise value is false.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined.
 *
 * The behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_default_constructible : etl::is_constructible<T>
{
};

template <typename T>
inline constexpr bool is_default_constructible_v
  = is_default_constructible<T>::value;

/**
 * @brief  If etl::is_trivially_constructible<T>::value is true, provides the
 * member constant value equal to true, otherwise value is false.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined.
 *
 * The behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_trivially_default_constructible : etl::is_trivially_constructible<T>
{
};

template <typename T>
inline constexpr bool is_trivially_default_constructible_v
  = is_trivially_default_constructible<T>::value;

/**
 * @brief If etl::is_nothrow_constructible<T>::value is true, provides the
 * member constant value equal to true, otherwise value is false.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined.
 *
 * The behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_nothrow_default_constructible : etl::is_nothrow_constructible<T>
{
};

template <typename T>
inline constexpr bool is_nothrow_default_constructible_v
  = is_nothrow_default_constructible<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_constructible<T, T const&>::value.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined.
 *
 * The behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_copy_constructible
    : etl::is_constructible<T, etl::add_lvalue_reference_t<etl::add_const_t<T>>>
{
};

template <typename T>
inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

/**
 * @brief Same as copy, but uses etl::is_trivially_constructible<T, T const&>.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined.
 *
 * The behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_trivially_copy_constructible
    : etl::is_trivially_constructible<
        T, etl::add_lvalue_reference_t<etl::add_const_t<T>>>
{
};

template <typename T>
inline constexpr bool is_trivially_copy_constructible_v
  = is_trivially_copy_constructible<T>::value;

/**
 * @brief Same as copy, but uses etl::is_nothrow_constructible<T, T const&>.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined.
 *
 * The behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_nothrow_copy_constructible
    : etl::is_nothrow_constructible<
        T, etl::add_lvalue_reference_t<etl::add_const_t<T>>>
{
};

template <typename T>
inline constexpr bool is_nothrow_copy_constructible_v
  = is_nothrow_copy_constructible<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_constructible<T, T&&>::value.
 */
template <typename T>
struct is_move_constructible
    : etl::is_constructible<T, etl::add_rvalue_reference_t<T>>
{
};

template <typename T>
inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_trivially_constructible<T, T&&>::value.
 */
template <typename T>
struct is_trivially_move_constructible
    : etl::is_trivially_constructible<T, etl::add_rvalue_reference_t<T>>
{
};

template <typename T>
inline constexpr bool is_trivially_move_constructible_v
  = is_trivially_move_constructible<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_nothrow_constructible<T, T&&>::value.
 */
template <typename T>
struct is_nothrow_move_constructible
    : etl::is_nothrow_constructible<T, etl::add_rvalue_reference_t<T>>
{
};

template <typename T>
inline constexpr bool is_nothrow_move_constructible_v
  = is_nothrow_move_constructible<T>::value;

namespace detail
{
struct try_is_destructible_impl
{
  template <typename T, typename = decltype(::etl::declval<T&>().~T())>
  static auto test(int) -> ::etl::true_type;

  template <typename>
  static auto test(...) -> ::etl::false_type;
};

template <typename T>
struct is_destructible_impl : public try_is_destructible_impl
{
  using type = decltype(test<T>(0));
};

template <
  typename T,
  bool = ::etl::disjunction<::etl::is_void<T>, ::etl::is_function<T>,
                            ::etl::is_unbounded_array<T>>::value,
  bool = ::etl::disjunction<::etl::is_reference<T>, ::etl::is_scalar<T>>::value>
struct is_destructible_safe;

template <typename T>
struct is_destructible_safe<T, false, false>
    : public is_destructible_impl<
        typename ::etl::remove_all_extents<T>::type>::type
{
};

template <typename T>
struct is_destructible_safe<T, true, false> : public ::etl::false_type
{
};

template <typename T>
struct is_destructible_safe<T, false, true> : public ::etl::true_type
{
};

}  // namespace detail

/**
 * @brief https://en.cppreference.com/w/cpp/types/is_destructible
 */
template <typename T>
struct is_destructible : public detail::is_destructible_safe<T>
{
  //   static_assert(
  //       etl::__is_complete_or_unbounded(__type_identity<T>{}),
  //       "template argument must be a complete class or an unbounded array");
};

/**
 * @brief https://en.cppreference.com/w/cpp/types/is_destructible
 */
template <typename Type>
struct is_destructible<Type[]> : public etl::false_type
{
};

/**
 * @brief https://en.cppreference.com/w/cpp/types/is_destructible
 */
template <>
struct is_destructible<void> : public etl::false_type
{
};

template <typename T>
inline constexpr auto is_destructible_v = is_destructible<T>::value;

/**
 * @brief https://en.cppreference.com/w/cpp/types/is_destructible
 */
template <typename T>
struct is_trivially_destructible
    : public bool_constant<TAETL_IS_TRIVIAL_DESTRUCTIBLE(T)>
{
};

template <typename T>
inline constexpr auto is_trivially_destructible_v
  = is_trivially_destructible<T>::value;

namespace detail
{
template <bool, typename Type>
struct is_nothrow_destructible_helper;

template <typename Type>
struct is_nothrow_destructible_helper<false, Type> : public ::etl::false_type
{
};

template <typename Type>
struct is_nothrow_destructible_helper<true, Type>
    : public ::etl::bool_constant<noexcept(::etl::declval<Type>().~Type())>
{
};
}  // namespace detail

/**
 * @brief https://en.cppreference.com/w/cpp/types/is_destructible
 */
template <typename Type>
struct is_nothrow_destructible : public detail::is_nothrow_destructible_helper<
                                   is_destructible<Type>::value, Type>
{
};

template <typename Type, size_t N>
struct is_nothrow_destructible<Type[N]> : public is_nothrow_destructible<Type>
{
};

template <typename Type>
struct is_nothrow_destructible<Type&> : public true_type
{
};

template <typename Type>
struct is_nothrow_destructible<Type&&> : public true_type
{
};

template <typename T>
inline constexpr bool is_nothrow_destructible_v
  = is_nothrow_destructible<T>::value;

/**
 * @brief https://en.cppreference.com/w/cpp/types/has_virtual_destructor
 */
template <typename T>
struct has_virtual_destructor
    : public bool_constant<TAETL_HAS_VIRTUAL_DESTRUCTOR(T)>
{
};

/**
 * @brief https://en.cppreference.com/w/cpp/types/has_virtual_destructor
 */
template <typename T>
inline constexpr auto has_virtual_destructor_v
  = has_virtual_destructor<T>::value;

/**
 * @brief If the expression etl::declval<T>() = etl::declval<U>() is well-formed
 * in unevaluated context, provides the member constant value equal true.
 * Otherwise, value is false. Access checks are performed as if from a context
 * unrelated to either type.
 */
template <typename T, typename U>
struct is_assignable : etl::bool_constant<TAETL_IS_ASSIGNABLE(T, U)>
{
};

template <typename T, typename U>
inline constexpr bool is_assignable_v = is_assignable<T, U>::value;

/**
 * @brief If the expression etl::declval<T>() = etl::declval<U>() is well-formed
 * in unevaluated context, provides the member constant value equal true.
 * Otherwise, value is false. Access checks are performed as if from a context
 * unrelated to either type.
 */
template <typename T, typename U>
struct is_trivially_assignable
    : etl::bool_constant<TAETL_IS_TRIVIALLY_ASSIGNABLE(T, U)>
{
};

template <typename T, typename U>
inline constexpr bool is_trivially_assignable_v
  = is_trivially_assignable<T, U>::value;

namespace detail
{
template <typename T, typename U>
struct is_nothrow_assignable_helper
    : public ::etl::bool_constant<noexcept(::etl::declval<T>()
                                           = ::etl::declval<U>())>
{
};
}  // namespace detail

/**
 * @brief If the expression etl::declval<T>() = etl::declval<U>() is well-formed
 * in unevaluated context, provides the member constant value equal true.
 * Otherwise, value is false. Access checks are performed as if from a context
 * unrelated to either type.
 */
template <typename T, typename U>
struct is_nothrow_assignable
    : public etl::integral_constant<
        bool, is_assignable_v<
                T, U> && detail::is_nothrow_assignable_helper<T, U>::value>
{
};

template <typename T, typename U>
inline constexpr bool is_nothrow_assignable_v
  = is_nothrow_assignable<T, U>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_assignable<T&, T const&>::value.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined. The
 * behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_copy_assignable
    : etl::is_assignable<typename etl::add_lvalue_reference<T>::type,
                         typename etl::add_lvalue_reference<const T>::type>
{
};

template <typename T>
inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_trivially_assignable<T&, T const&>::value.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined. The
 * behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_trivially_copy_assignable
    : etl::is_trivially_assignable<
        typename etl::add_lvalue_reference<T>::type,
        typename etl::add_lvalue_reference<const T>::type>
{
};

template <typename T>
inline constexpr bool is_trivially_copy_assignable_v
  = is_trivially_copy_assignable<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_nothrow_assignable<T&, T const&>::value.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined. The
 * behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_nothrow_copy_assignable
    : etl::is_nothrow_assignable<
        typename etl::add_lvalue_reference<T>::type,
        typename etl::add_lvalue_reference<const T>::type>
{
};

template <typename T>
inline constexpr bool is_nothrow_copy_assignable_v
  = is_nothrow_copy_assignable<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_assignable<T&, T&&>::value.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined. The
 * behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_move_assignable
    : etl::is_assignable<typename etl::add_lvalue_reference<T>::type,
                         typename etl::add_rvalue_reference<T>::type>
{
};

template <typename T>
inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_assignable<T&, T&&>::value.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined. The
 * behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_trivially_move_assignable
    : etl::is_trivially_assignable<typename etl::add_lvalue_reference<T>::type,
                                   typename etl::add_rvalue_reference<T>::type>
{
};

template <typename T>
inline constexpr bool is_trivially_move_assignable_v
  = is_trivially_move_assignable<T>::value;

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_assignable<T&, T&&>::value.
 *
 * @details T shall be a complete type, (possibly cv-qualified) void, or an
 * array of unknown bound. Otherwise, the behavior is undefined. If an
 * instantiation of a template above depends, directly or indirectly, on an
 * incomplete type, and that instantiation could yield a different result if
 * that type were hypothetically completed, the behavior is undefined. The
 * behavior of a program that adds specializations for any of the templates
 * described on this page is undefined.
 */
template <typename T>
struct is_nothrow_move_assignable
    : etl::is_nothrow_assignable<typename etl::add_lvalue_reference<T>::type,
                                 typename etl::add_rvalue_reference<T>::type>
{
};

template <typename T>
inline constexpr bool is_nothrow_move_assignable_v
  = is_nothrow_move_assignable<T>::value;

namespace detail
{
struct nat
{
  nat()           = delete;
  nat(nat const&) = delete;
  auto operator=(nat const&) -> nat& = delete;
  ~nat()                             = delete;
};

using ::etl::swap;
template <typename T>
void swap(nat a, nat b) noexcept;

template <typename T>
struct is_swappable_helper
{
  using type = decltype(swap(::etl::declval<T&>(), ::etl::declval<T&>()));
  static const bool value = !::etl::is_same_v<type, nat>;
};

}  // namespace detail

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_swappable_with<T&, T&>::value
 */
template <typename T>
struct is_swappable
    : public bool_constant<detail::is_swappable_helper<T>::value>
{
};

template <typename T>
inline constexpr bool is_swappable_v = is_swappable<T>::value;

namespace detail
{
template <bool, typename T>
struct is_nothrow_swappable_helper
    : public ::etl::bool_constant<noexcept(
        swap(::etl::declval<T&>(), ::etl::declval<T&>()))>
{
};
template <typename T>
struct is_nothrow_swappable_helper<false, T> : public ::etl::false_type
{
};
}  // namespace detail

/**
 * @brief If T is not a referenceable type (i.e., possibly cv-qualified void or
 * a function type with a cv-qualifier-seq or a ref-qualifier), provides a
 * member constant value equal to false. Otherwise, provides a member constant
 * value equal to etl::is_nothrow_swappable_with<T&, T&>::value
 */
template <typename T>
struct is_nothrow_swappable
    : public detail::is_nothrow_swappable_helper<is_swappable<T>::value, T>
{
};

template <typename T>
inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

/**
 * @brief alignment_of
 */
template <typename T>
struct alignment_of : public integral_constant<size_t, alignof(T)>
{
};

template <typename T>
inline constexpr size_t alignment_of_v = alignment_of<T>::value;

/**
 * @brief If T is a TriviallyCopyable type, provides the member constant value
 * equal to true. For any other type, value is false. The only trivially
 * copyable types are scalar types, trivially copyable classes, and arrays of
 * such types/classes (possibly cv-qualified).
 */
template <typename T>
class is_trivially_copyable
{
  // copy constructors
  static constexpr bool has_trivial_copy_ctor = etl::is_copy_constructible_v<T>;
  static constexpr bool has_deleted_copy_ctor
    = !etl::is_copy_constructible_v<T>;

  // move constructors
  static constexpr bool has_trivial_move_ctor = etl::is_move_constructible_v<T>;
  static constexpr bool has_deleted_move_ctor
    = !etl::is_move_constructible_v<T>;

  // copy assign
  static constexpr bool has_trivial_copy_assign = is_copy_assignable_v<T>;
  static constexpr bool has_deleted_copy_assign = !is_copy_assignable_v<T>;

  // move assign
  static constexpr bool has_trivial_move_assign = is_move_assignable_v<T>;
  static constexpr bool has_deleted_move_assign = !is_move_assignable_v<T>;

  // destructor
  static constexpr bool has_trivial_dtor = etl::is_destructible_v<T>;

  public:
  static constexpr bool value
    = has_trivial_dtor && (has_deleted_move_assign || has_trivial_move_assign)
      && (has_deleted_move_ctor || has_trivial_move_ctor)
      && (has_deleted_copy_assign || has_trivial_copy_assign)
      && (has_deleted_copy_ctor || has_trivial_copy_ctor);
};

template <typename T>
class is_trivially_copyable<T*> : public etl::true_type
{
};

template <typename T>
inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

/**
 * @brief If T is TrivialType (that is, a scalar type, a trivially copyable
 * class with a trivial default constructor, or array of such type/class,
 * possibly cv-qualified), provides the member constant value equal to true. For
 * any other type, value is false.
 *
 * @details https://en.cppreference.com/w/cpp/types/is_trivial
 */
template <typename T>
struct is_trivial
    : etl::bool_constant<etl::is_trivially_copyable<T>::value
                         && etl::is_trivially_default_constructible<T>::value>
{
};

template <typename T>
inline constexpr bool is_trivial_v = is_trivial<T>::value;

namespace detail
{
template <typename T, bool = ::etl::is_arithmetic_v<T>>
struct is_unsigned : ::etl::bool_constant<T(0) < T(-1)>
{
};

template <typename T>
struct is_unsigned<T, false> : ::etl::false_type
{
};
}  // namespace detail

/**
 * @brief If T is an arithmetic type, provides the member constant value equal
 * to true if T(0) < T(-1): this results in true for the unsigned integer types
 * and the type bool and in false for the signed integer types and the
 * floating-point types. For any other type, value is false. The behavior of a
 * program that adds specializations for is_unsigned or is_unsigned_v (since
 * C++17) is undefined.
 */
template <typename T>
struct is_unsigned : detail::is_unsigned<T>::type
{
};

template <typename T>
inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

namespace detail
{
template <typename T, bool = ::etl::is_arithmetic_v<T>>
struct is_signed : ::etl::bool_constant<T(-1) < T(0)>
{
};

template <typename T>
struct is_signed<T, false> : ::etl::false_type
{
};
}  // namespace detail

/**
 * @brief If T is an arithmetic type, provides the member constant value equal
 * to true if T(-1) < T(0): this results in true for the floating-point types
 * and the signed integer types, and in false for the unsigned integer types and
 * the type bool. For any other type, value is false.
 */
template <typename T>
struct is_signed : detail::is_signed<T>::type
{
};

template <typename T>
inline constexpr bool is_signed_v = is_signed<T>::value;

namespace detail
{
template <typename B>
auto test_pre_ptr_convertible(B const volatile*) -> ::etl::true_type;
template <typename>
auto test_pre_ptr_convertible(void const volatile*) -> ::etl::false_type;

template <typename, typename>
auto test_pre_is_base_of(...) -> ::etl::true_type;
template <typename B, typename D>
auto test_pre_is_base_of(int)
  -> decltype(test_pre_ptr_convertible<B>(static_cast<D*>(nullptr)));
}  // namespace detail

/**
 * @brief If Derived is derived from Base or if both are the same non-union
 * class (in both cases ignoring cv-qualification), provides the member constant
 * value equal to true. Otherwise value is false.
 *
 * @details If both Base and Derived are non-union class types, and they are not
 * the same type (ignoring cv-qualification), Derived shall be a complete type;
 * otherwise the behavior is undefined.
 *
 * @details https://en.cppreference.com/w/cpp/types/is_base_of
 */
template <typename Base, typename Derived>
struct is_base_of
    : etl::bool_constant<etl::is_class<Base>::value
                         && etl::is_class<Derived>::value&& decltype(
                           detail::test_pre_is_base_of<Base, Derived>(
                             0))::value>
{
};

template <typename Base, typename Derived>
inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

/**
 * @brief If Type is an array type, provides the member constant value equal to
 * the number of dimensions of the array. For any other type, value is 0. The
 * behavior of a program that adds specializations for rank or rank_v is
 * undefined.
 */
template <typename T>
struct rank : public etl::integral_constant<etl::size_t, 0>
{
};

template <typename T>
struct rank<T[]>
    : public etl::integral_constant<etl::size_t, rank<T>::value + 1>
{
};

template <typename T, etl::size_t N>
struct rank<T[N]>
    : public etl::integral_constant<etl::size_t, rank<T>::value + 1>
{
};

template <typename Type>
inline constexpr etl::size_t rank_v = rank<Type>::value;

/**
 * Applies lvalue-to-rvalue, array-to-pointer, and function-to-pointer implicit
 * conversions to the type T, removes cv-qualifiers, and defines the resulting
 * type as the member typedef type.
 */
template <typename T>
struct decay
{
  private:
  using U = typename etl::remove_reference<T>::type;

  public:
  using type = typename etl::conditional<
    etl::is_array<U>::value, typename etl::remove_extent<U>::type*,
    typename etl::conditional<etl::is_function<U>::value,
                              typename etl::add_pointer<U>::type,
                              typename etl::remove_cv<U>::type>::type>::type;
};

template <typename T>
using decay_t = typename decay<T>::type;

/**
 * @brief Determines the common type among all types T..., that is the type all
 * T... can be implicitly converted to. If such a type exists (as determined
 * according to the rules below), the member type names that type. Otherwise,
 * there is no member type.
 * @details https://en.cppreference.com/w/cpp/types/common_type
 */
// primary template (used for zero types)
template <typename...>
struct common_type
{
};

//////// one type
template <typename T>
struct common_type<T> : common_type<T, T>
{
};

namespace detail
{
template <typename T1, typename T2>
using cond_t = decltype(false ? ::etl::declval<T1>() : ::etl::declval<T2>());

template <typename T1, typename T2, typename = void>
struct common_type_2_impl
{
};

template <typename T1, typename T2>
struct common_type_2_impl<T1, T2, void_t<cond_t<T1, T2>>>
{
  using type = ::etl::decay_t<cond_t<T1, T2>>;
};

template <typename AlwaysVoid, typename T1, typename T2, typename... R>
struct common_type_multi_impl
{
};

template <typename T1, typename T2, typename... R>
struct common_type_multi_impl<void_t<typename common_type<T1, T2>::type>, T1,
                              T2, R...>
    : common_type<typename common_type<T1, T2>::type, R...>
{
};
}  // namespace detail

//////// two types

template <typename T1, typename T2>
struct common_type<T1, T2>
    : detail::common_type_2_impl<typename etl::decay<T1>::type,
                                 typename etl::decay<T2>::type>
{
};

//////// 3+ types
template <typename T1, typename T2, typename... R>
struct common_type<T1, T2, R...>
    : detail::common_type_multi_impl<void, T1, T2, R...>
{
};

template <typename... T>
using common_type_t = typename common_type<T...>::type;

namespace detail
{
template <typename>
using true_type_for = ::etl::true_type;

template <typename T>
auto test_returnable(int) -> true_type_for<T()>;
template <typename>
auto test_returnable(...) -> ::etl::false_type;

template <typename From, typename To>
auto test_nonvoid_convertible(int) -> true_type_for<
  decltype(::etl::declval<void (&)(To)>()(::etl::declval<From>()))>;
template <typename, typename>
auto test_nonvoid_convertible(...) -> ::etl::false_type;

}  // namespace detail

/**
 * @brief If the imaginary function definition To test() { return
 * etl::declval<From>(); } is well-formed, (that is, either etl::declval<From>()
 * can be converted to To using implicit conversions, or both From and To are
 * possibly cv-qualified void), provides the member constant value equal to
 * true. Otherwise value is false. For the purposes of this check, the use of
 * etl::declval in the return statement is not considered an odr-use. Access
 * checks are performed as if from a context unrelated to either type. Only the
 * validity of the immediate context of the expression in the return statement
 * (including conversions to the return type) is considered.
 */
template <typename From, typename To>
struct is_convertible
    : etl::integral_constant<
        bool, (decltype(detail::test_returnable<To>(0))::value&& decltype(
                detail::test_nonvoid_convertible<From, To>(0))::value)
                || (etl::is_void<From>::value && etl::is_void<To>::value)>
{
};

template <typename From, typename To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

/**
 * @brief Provides the nested type type, which is a trivial standard-layout type
 * suitable for use as uninitialized storage for any object whose size is at
 * most Len and whose alignment requirement is a divisor of Align.
 *
 * @details The default value of Align is the most stringent (the largest)
 * alignment requirement for any object whose size is at most Len. If the
 * default value is not used, Align must be the value of alignof(T) for some
 * type T, or the behavior is undefined.
 */
template <etl::size_t Len, etl::size_t Align = alignof(etl::max_align_t)>
struct aligned_storage
{
  struct type
  {
    alignas(Align) unsigned char data[Len];
  };
};

template <etl::size_t Len, etl::size_t Align = alignof(etl::max_align_t)>
using aligned_storage_t = typename aligned_storage<Len, Align>::type;

/**
 * @brief Detects whether the function call occurs within a constant-evaluated
 * context. Returns true if the evaluation of the call occurs within the
 * evaluation of an expression or conversion that is manifestly
 * constant-evaluated; otherwise returns false.
 *
 * https://en.cppreference.com/w/cpp/types/is_constant_evaluated
 */
[[nodiscard]] inline constexpr auto is_constant_evaluated() noexcept -> bool
{
#if defined(TAETL_CPP_STANDARD_20)
  return TAETL_IS_CONSTANT_EVALUATED();
#else
  return false;
#endif
}

}  // namespace etl

#endif  // TAETL_TYPETRAITS_HPP