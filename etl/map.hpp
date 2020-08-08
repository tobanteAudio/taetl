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

#ifndef TAETL_MAP_HPP
#define TAETL_MAP_HPP

#include "etl/definitions.hpp"
#include "etl/functional.hpp"
#include "etl/utility.hpp"

namespace etl
{
template <typename KeyType, typename ValueType,
          typename Compare = etl::less<KeyType>>
class map
{
public:
    using key_type        = KeyType;
    using mapped_type     = ValueType;
    using value_type      = etl::pair<const KeyType, ValueType>;
    using size_type       = etl::size_t;
    using difference_type = etl::ptrdiff_t;
    using key_compare     = Compare;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using pointer         = value_type*;
    using const_pointer   = value_type const*;
    using iterator        = value_type*;
    using const_iterator  = value_type const*;

    // TODO: Reverse iterators & node type
    // reverse_iterator std::reverse_iterator<iterator>
    // const_reverse_iterator std::reverse_iterator<const_iterator>
    // node_type a specialization of node handle representing a container node
    // insert_return_type

    /**
     * @brief Returns a reference to the mapped value of the element with key
     * equivalent to key. If no such element exists, you are in UB land.
     */
    [[nodiscard]] constexpr auto at(key_type const& key) -> mapped_type&
    {
        assert(find(key) != nullptr);
        return find(key)->second;
    }

    /**
     * @brief Returns a reference to the mapped value of the element with key
     * equivalent to key. If no such element exists, you are in UB land.
     */
    [[nodiscard]] constexpr auto at(key_type const& key) const
        -> mapped_type const&
    {
        assert(find(key) != nullptr);
        return find(key)->second;
    }

    /**
     * @brief Returns a reference to the value that is mapped to a key
     * equivalent to key, performing an insertion if such key does not already
     * exist.
     */
    [[nodiscard]] constexpr auto operator[](key_type const& key) -> mapped_type&
    {
        auto const item = find(key);
        if (item == nullptr)
        {
            auto const res = insert(value_type {key, {}});
            return res.first->second;
        }
        return item->second;
    }

    /**
     * @brief Returns an iterator to the beginning.
     */
    [[nodiscard]] constexpr auto begin() noexcept -> iterator { return data_; }

    /**
     * @brief Returns an const iterator to the beginning.
     */
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
    {
        return data_;
    }

    /**
     * @brief Returns an const iterator to the beginning.
     */
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
    {
        return data_;
    }

    /**
     * @brief Returns an iterator to the end.
     */
    [[nodiscard]] constexpr auto end() noexcept -> iterator
    {
        return data_ + size();
    }

    /**
     * @brief Returns an const iterator to the end.
     */
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator
    {
        return data_ + size();
    }

    /**
     * @brief Returns an const iterator to the end.
     */
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator
    {
        return data_ + size();
    }

    /**
     * @brief Returns the current element count.
     */
    [[nodiscard]] constexpr auto size() const noexcept -> size_type
    {
        return size_;
    }

    /**
     * @brief Returns the capacity.
     */
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type
    {
        return capacity_;
    }

    /**
     * @brief Returns true if the size == 0.
     */
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return size_ == 0;
    }

    /**
     * @brief Returns 1 if the key is present, otherwise 0.
     */
    [[nodiscard]] constexpr auto count(key_type const& key) const noexcept
        -> size_type
    {
        return find(key) != nullptr ? 1 : 0;
    }

    /**
     * @brief  Checks if there is an element with key equivalent to key in the
     * container.
     */
    [[nodiscard]] constexpr auto contains(key_type const& key) const -> bool
    {
        return find(key) != nullptr;
    }

    /**
     * @brief Erases all elements from the container. After this call, size()
     * returns zero.
     */
    constexpr auto clear() noexcept -> void
    {
        // TODO: Replace with calls to destructor
        // ::memset(data_, 0, capacity_ * sizeof(value_type));
        size_ = 0;
    }

    /**
     * @brief Inserts a value pair into the map. Returns a pair consisting of an
     * iterator to the inserted element (or to the element that prevented the
     * insertion) and a bool denoting whether the insertion took place.
     */
    constexpr auto insert(value_type const& value) noexcept
        -> etl::pair<iterator, bool>
    {
        if (size_ == capacity_) { return {end(), false}; }

        auto* const addr = reinterpret_cast<void*>(&data_[size_++]);
        ::new (addr) value_type {value};
        return {&data_[size_], true};
    }

    /**
     * @brief Inserts a value pair into the map. Returns a pair consisting of an
     * iterator to the inserted element (or to the element that prevented the
     * insertion) and a bool denoting whether the insertion took place.
     */
    constexpr auto insert(value_type&& value) -> etl::pair<iterator, bool>
    {
        if (size_ == capacity_) { return {end(), false}; }

        auto* const addr = reinterpret_cast<void*>(&data_[size_++]);
        ::new (addr) value_type {value};
        return {&data_[size_], true};
    }

    /**
     * @brief Returns an element with key equivalent to key.
     */
    [[nodiscard]] constexpr auto find(KeyType const& key) noexcept -> iterator
    {
        for (auto i = begin(); i != end(); ++i)
        {
            if (i->first == key) { return i; }
        }
        return nullptr;
    }

    /**
     * @brief Returns an element with key equivalent to key.
     */
    [[nodiscard]] constexpr auto find(KeyType const& key) const noexcept
        -> const_iterator
    {
        for (auto i = cbegin(); i != cend(); ++i)
        {
            if (i->first == key) { return i; }
        }

        return nullptr;
    }

protected:
    explicit constexpr map(pointer data, size_t capacity)
        : data_(data), size_(0), capacity_(capacity)
    {
    }

private:
    pointer data_;
    size_type size_;
    size_type const capacity_;
};

namespace make
{
template <typename KeyType, typename ValueType, size_t Capacity,
          typename Compare = etl::less<KeyType>>
class map : public ::etl::map<KeyType, ValueType, Compare>
{
public:
    explicit map()
        : ::etl::map<KeyType, ValueType, Compare> {&data_[0], Capacity}
    {
    }

private:
    typename ::etl::map<KeyType, ValueType, Compare>::value_type
        data_[Capacity];
};
}  // namespace make
}  // namespace etl
#endif  // TAETL_MAP_HPP