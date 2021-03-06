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

#ifndef TAETL_TYPETRAITS_DECL_HPP
#define TAETL_TYPETRAITS_DECL_HPP

#include "etl/detail/intrinsics.hpp"

namespace etl
{
// primary type categories:
template <class T>
struct is_void;
template <class T>
struct is_null_pointer;
template <class T>
struct is_integral;
template <class T>
struct is_floating_point;
template <class T>
struct is_array;
template <class T>
struct is_pointer;
template <class T>
struct is_lvalue_reference;
template <class T>
struct is_rvalue_reference;
template <class T>
struct is_member_object_pointer;
template <class T>
struct is_member_function_pointer;
template <class T>
struct is_enum;
template <class T>
struct is_union;
template <class T>
struct is_class;
template <class T>
struct is_function;

// composite type categories:
template <class T>
struct is_reference;
template <class T>
struct is_arithmetic;
template <class T>
struct is_fundamental;
template <class T>
struct is_object;
template <class T>
struct is_scalar;
template <class T>
struct is_compound;
template <class T>
struct is_member_pointer;

template <class T>
constexpr auto swap(T& a, T& b) noexcept -> void;

}  // namespace etl

#endif  // TAETL_TYPETRAITS_DECL_HPP