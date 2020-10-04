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

#include "etl/cctype.hpp"

#include "catch2/catch.hpp"

TEST_CASE("cctype: isalnum", "[cctype]")
{
    SECTION("digit")
    {
        CHECK(etl::isalnum('0'));
        CHECK(etl::isalnum('1'));
        CHECK(etl::isalnum('2'));
        CHECK(etl::isalnum('3'));
        CHECK(etl::isalnum('4'));
        CHECK(etl::isalnum('5'));
        CHECK(etl::isalnum('6'));
        CHECK(etl::isalnum('7'));
        CHECK(etl::isalnum('8'));
        CHECK(etl::isalnum('9'));
    }

    SECTION("lower")
    {
        for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK(etl::isalnum(ch)); }
    }

    SECTION("upper")
    {
        for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK(etl::isalnum(ch)); }
    }
}

TEST_CASE("cctype: isalpha", "[cctype]")
{
    SECTION("true")
    {
        for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK(etl::isalpha(ch)); }
        for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK(etl::isalpha(ch)); }
    }

    SECTION("false")
    {
        CHECK_FALSE(etl::isalpha('0'));
        CHECK_FALSE(etl::isalpha('1'));
        CHECK_FALSE(etl::isalpha('2'));
        CHECK_FALSE(etl::isalpha('3'));
        CHECK_FALSE(etl::isalpha('4'));
        CHECK_FALSE(etl::isalpha('5'));
        CHECK_FALSE(etl::isalpha('6'));
        CHECK_FALSE(etl::isalpha('7'));
        CHECK_FALSE(etl::isalpha('8'));
        CHECK_FALSE(etl::isalpha('9'));
    }
}

TEST_CASE("cctype: islower", "[cctype]")
{
    for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK(etl::islower(ch)); }
    for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK_FALSE(etl::islower(ch)); }
    for (auto ch = '0'; ch <= '9'; ++ch) { CHECK_FALSE(etl::islower(ch)); }
}

TEST_CASE("cctype: isupper", "[cctype]")
{
    for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK(etl::isupper(ch)); }
    for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK_FALSE(etl::isupper(ch)); }
    for (auto ch = '0'; ch <= '9'; ++ch) { CHECK_FALSE(etl::isupper(ch)); }
}

TEST_CASE("cctype: isdigit", "[cctype]")
{
    for (auto ch = '0'; ch <= '9'; ++ch) { CHECK(etl::isdigit(ch)); }
    for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK_FALSE(etl::isdigit(ch)); }
    for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK_FALSE(etl::isdigit(ch)); }
}

TEST_CASE("cctype: isxdigit", "[cctype]")
{
    for (auto ch = '0'; ch <= '9'; ++ch) { CHECK(etl::isxdigit(ch)); }
    for (auto ch = 'A'; ch <= 'F'; ++ch) { CHECK(etl::isxdigit(ch)); }
    for (auto ch = 'a'; ch <= 'f'; ++ch) { CHECK(etl::isxdigit(ch)); }

    for (auto ch = 'G'; ch <= 'Z'; ++ch) { CHECK_FALSE(etl::isxdigit(ch)); }
    for (auto ch = 'g'; ch <= 'z'; ++ch) { CHECK_FALSE(etl::isxdigit(ch)); }
}

TEST_CASE("cctype: isspace", "[cctype]")
{
    CHECK(etl::isspace(' '));
    CHECK(etl::isspace('\f'));
    CHECK(etl::isspace('\n'));
    CHECK(etl::isspace('\r'));
    CHECK(etl::isspace('\t'));
    CHECK(etl::isspace('\v'));

    for (auto ch = '0'; ch <= '9'; ++ch) { CHECK_FALSE(etl::isspace(ch)); }
    for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK_FALSE(etl::isspace(ch)); }
    for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK_FALSE(etl::isspace(ch)); }
}

TEST_CASE("cctype: isblank", "[cctype]")
{
    CHECK(etl::isblank(' '));
    CHECK(etl::isblank('\t'));

    CHECK_FALSE(etl::isblank('\f'));
    CHECK_FALSE(etl::isblank('\n'));
    CHECK_FALSE(etl::isblank('\r'));
    CHECK_FALSE(etl::isblank('\v'));

    for (auto ch = '0'; ch <= '9'; ++ch) { CHECK_FALSE(etl::isblank(ch)); }
    for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK_FALSE(etl::isblank(ch)); }
    for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK_FALSE(etl::isblank(ch)); }
}

TEST_CASE("cctype: ispunct", "[cctype]")
{
    for (auto ch = '!'; ch <= '/'; ++ch) { CHECK(etl::ispunct(ch)); }
    for (auto ch = ':'; ch <= '@'; ++ch) { CHECK(etl::ispunct(ch)); }
    for (auto ch = '['; ch <= '`'; ++ch) { CHECK(etl::ispunct(ch)); }
    for (auto ch = '{'; ch <= '~'; ++ch) { CHECK(etl::ispunct(ch)); }

    for (auto ch = '0'; ch <= '9'; ++ch) { CHECK_FALSE(etl::ispunct(ch)); }
    for (auto ch = 'a'; ch <= 'z'; ++ch) { CHECK_FALSE(etl::ispunct(ch)); }
    for (auto ch = 'A'; ch <= 'Z'; ++ch) { CHECK_FALSE(etl::ispunct(ch)); }
}

TEST_CASE("cctype: tolower", "[cctype]")
{
    CHECK(static_cast<char>(etl::tolower('a')) == 'a');
    CHECK(static_cast<char>(etl::tolower('A')) == 'a');

    for (auto ch = '0'; ch <= '9'; ++ch)
    { CHECK(static_cast<char>(etl::tolower(ch)) == ch); }

    for (auto ch = 'a'; ch <= 'z'; ++ch)
    { CHECK(static_cast<char>(etl::tolower(ch)) == ch); }

    for (auto ch = 'A'; ch <= 'Z'; ++ch)
    { CHECK(static_cast<char>(etl::tolower(ch)) == ch + 32); }
}

TEST_CASE("cctype: toupper", "[cctype]")
{
    CHECK(static_cast<char>(etl::toupper('a')) == 'A');
    CHECK(static_cast<char>(etl::toupper('A')) == 'A');

    for (auto ch = '0'; ch <= '9'; ++ch)
    { CHECK(static_cast<char>(etl::toupper(ch)) == ch); }

    for (auto ch = 'A'; ch <= 'Z'; ++ch)
    { CHECK(static_cast<char>(etl::toupper(ch)) == ch); }

    for (auto ch = 'a'; ch <= 'z'; ++ch)
    { CHECK(static_cast<char>(etl::toupper(ch)) == ch - 32); }
}