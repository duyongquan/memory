// Copyright (C) 2015-2016 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_MEMORY_SRC_DETAIL_FREE_LIST_UTILS_HPP_INCLUDED
#define FOONATHAN_MEMORY_SRC_DETAIL_FREE_LIST_UTILS_HPP_INCLUDED

#include <cstdint>

#include "config.hpp"
#include "detail/align.hpp"
#include "detail/assert.hpp"

#if FOONATHAN_HOSTED_IMPLEMENTATION
    #include <functional>
#endif

namespace foonathan { namespace memory
{
    namespace detail
    {
        //=== storage ===///
        // reads stored integer value
        inline std::uintptr_t get_int(void *address) FOONATHAN_NOEXCEPT
        {
            FOONATHAN_MEMORY_ASSERT(address);
            FOONATHAN_MEMORY_ASSERT(is_aligned(address, FOONATHAN_ALIGNOF(std::uintptr_t)));
            return *static_cast<std::uintptr_t *>(address);
        }

        // sets stored integer value
        inline void set_int(void *address, std::uintptr_t i) FOONATHAN_NOEXCEPT
        {
            FOONATHAN_MEMORY_ASSERT(address);
            FOONATHAN_MEMORY_ASSERT(is_aligned(address, FOONATHAN_ALIGNOF(std::uintptr_t)));
            *static_cast<std::uintptr_t *>(address) = i;
        }

        // pointer to integer
        inline std::uintptr_t to_int(char *ptr) FOONATHAN_NOEXCEPT
        {
            return reinterpret_cast<std::uintptr_t>(ptr);
        }

        // integer to pointer
        inline char *from_int(std::uintptr_t i) FOONATHAN_NOEXCEPT
        {
            return reinterpret_cast<char *>(i);
        }

        //=== intrusive linked list ===//
        // reads a stored pointer value
        inline char *list_get_next(void *address) FOONATHAN_NOEXCEPT
        {
            return from_int(get_int(address));
        }

        // stores a pointer value
        inline void list_set_next(void *address, char *ptr) FOONATHAN_NOEXCEPT
        {
            set_int(address, to_int(ptr));
        }

        //=== intrusive xor linked list ===//
        // returns the other pointer given one pointer
        inline char *xor_list_get_other(void *address, char *prev_or_next) FOONATHAN_NOEXCEPT
        {
            return from_int(get_int(address) ^ to_int(prev_or_next));
        }

        // sets the next and previous pointer (order actually does not matter)
        inline void xor_list_set(void *address, char *prev, char *next) FOONATHAN_NOEXCEPT
        {
            set_int(address, to_int(prev) ^ to_int(next));
        }

        // changes other pointer given one pointer
        inline void xor_list_change(void *address, char *old_ptr, char *new_ptr) FOONATHAN_NOEXCEPT
        {
            FOONATHAN_MEMORY_ASSERT(address);
            auto other = xor_list_get_other(address, old_ptr);
            xor_list_set(address, other, new_ptr);
        }

        // advances a pointer pair forward/backward
        inline void xor_list_iter_next(char *&cur, char *&prev) FOONATHAN_NOEXCEPT
        {
            auto next = xor_list_get_other(cur, prev);
            prev = cur;
            cur = next;
        }

        // links new node between prev and next
        inline void xor_list_insert(char *new_node, char *prev, char *next) FOONATHAN_NOEXCEPT
        {
            xor_list_set(new_node, prev, next);
            xor_list_change(prev, next, new_node); // change prev's next to new_node
            xor_list_change(next, prev, new_node); // change next's prev to new_node
        }

        //=== sorted list utils ===//
        // if std::less/std::greater not available compare integer representation and hope it works
        bool less(char *a, char *b) FOONATHAN_NOEXCEPT
        {
#if FOONATHAN_HOSTED_IMPLEMENTATION
            return std::less<char*>()(a, b);
#else
            return to_int(a) < to_int(b);
#endif
        }

        bool greater(char *a, char *b) FOONATHAN_NOEXCEPT
        {
#if FOONATHAN_HOSTED_IMPLEMENTATION
            return std::greater<char*>()(a, b);
#else
            return to_int(a) < to_int(b);
#endif
        }
    } // namespace detail
}} // namespace foonathan::memory

#endif // FOONATHAN_MEMORY_SRC_DETAIL_FREE_LIST_UTILS_HPP_INCLUDED
