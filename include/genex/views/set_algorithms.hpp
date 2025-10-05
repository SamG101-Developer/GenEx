#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/algorithms/contains.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::concepts {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    concept set_algorithmicable_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::indirectly_comparable<I1, I2, Comp, Proj1, Proj2> and
        std::movable<I1> and std::movable<S1> and
        std::movable<I2> and std::movable<S2>;

    template <typename Rng1, typename Rng2, typename Comp, typename Proj1, typename Proj2>
    concept set_algorithmicable_range =
        input_range<Rng1> and
        input_range<Rng2> and
        set_algorithmicable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Comp, Proj1, Proj2>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, Comp, Proj1, Proj2>
    struct set_difference_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_difference_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_difference_iterator);

        I2 it2;
        S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_difference_iterator(I it, S st, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> set_difference_iterator& {
            ++it;
            satisfy();
            return *this;
        }

    private:
        GENEX_INLINE auto satisfy() const -> void {
            while (it != st) {
                // Advance it2 until either it2 == st2 or greater then the current value.
                while (it2 != st2 and std::invoke(comp, std::invoke(proj2, *it2), std::invoke(proj1, *it))) {
                    ++it2;
                }

                // If we reached the end of it2 or the current value is less than the current value of it2, we are done.
                if (it2 == st2 or std::invoke(comp, std::invoke(proj1, *it), std::invoke(proj2, *it2))) {
                    break;
                }

                // Otherwise, the current value is equal to the current value of it2, so we need to advance it.
                if (it2 != st2 and not std::invoke(comp, std::invoke(proj1, *it), std::invoke(proj2, *it2)) and not std::invoke(comp, std::invoke(proj2, *it2), std::invoke(proj1, *it))) {
                    ++it;
                }
            }
        }
    };

    template <typename I, typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, Comp, Proj1, Proj2>
    struct set_intersection_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_intersection_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_intersection_iterator);

        I2 it2;
        S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_intersection_iterator(I it, S st, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and noexcept(++it2))
            -> set_intersection_iterator& {
            ++it;
            if (it != st) {
                ++it2;
                satisfy();
            }
            return *this;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it != st and it2 != st2) {
                auto &&v1 = std::invoke(proj1, *it);
                auto &&v2 = std::invoke(proj2, *it2);

                if (std::invoke(comp, v1, v2)) {
                    ++it;
                }
                else if (std::invoke(comp, v1, v2)) {
                    ++it2;
                }
                else {
                    break;
                }
            }

            if (it == st or it2 == st2) {
                it = st;
            }
        }
    };


    template <typename I, typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, Comp, Proj1, Proj2>
    struct set_symmetric_difference_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_symmetric_difference_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_symmetric_difference_iterator);

        I2 it2;
        S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        bool from_first = true;

        GENEX_INLINE constexpr explicit set_symmetric_difference_iterator(I it, S st, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it) and noexcept(*it2))
            -> reference {
            return from_first ? *it : *it2;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)) and noexcept(std::addressof(*it2)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and noexcept(++it2))
            -> set_symmetric_difference_iterator& {
            satisfy();
            return *this;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it != st and it2 != st2) {
                if (it == st) {
                    from_first = false;
                    ++it2;
                    return;
                }
                if (it2 == st2) {
                    from_first = true;
                    ++it;
                    return;
                }

                auto &&v1 = std::invoke(proj1, *it);
                auto &&v2 = std::invoke(proj2, *it2);

                if (std::invoke(comp, v1, v2)) {
                    from_first = true;
                    ++it;
                    return;
                }
                else if (std::invoke(comp, v2, v1)) {
                    from_first = false;
                    ++it2;
                    return;
                }
                else {
                    ++it;
                    ++it2;
                }
            }

            if (it == st and it2 != st2) {
                it = st;
            }
        }
    };


    template <typename I, typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, Comp, Proj1, Proj2>
    struct set_union_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_union_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_union_iterator);

        I2 it2;
        S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        bool from_first = true;

        GENEX_INLINE constexpr explicit set_union_iterator(I it, S st, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it) and noexcept(*it2))
            -> reference {
            return from_first ? *it : *it2;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)) and noexcept(std::addressof(*it2)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and noexcept(++it2))
            -> set_union_iterator& {
            satisfy();
            return *this;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it != st or it2 != st2) {
                auto &&v1 = std::invoke(proj1, *it);
                auto &&v2 = std::invoke(proj2, *it2);

                if (std::invoke(comp, v1, v2)) {
                    from_first = true;
                    ++it;
                    return;
                }
                else if (std::invoke(comp, v2, v1)) {
                    from_first = false;
                    ++it2;
                    return;
                }
                else {
                    from_first = true;
                    ++it;
                    ++it2;
                    return;
                }
            }
        }
    };

    template <typename I, typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, operations::eq, Proj1, Proj2>
    struct set_difference_unsorted_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_difference_unsorted_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_difference_unsorted_iterator);

        I2 it2;
        S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_difference_unsorted_iterator(I it, S st, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> set_difference_unsorted_iterator& {
            ++it;
            satisfy();
            return *this;
        }

    private:
        auto satisfy() -> void {
            while (it != st) {
                if (not algorithms::contains(it2, st2, std::invoke(proj1, *it), proj2)) {
                    break;
                }
                ++it;
            }
        }
    };

    template <typename I, typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, operations::eq, Proj1, Proj2>
    struct set_intersection_unsorted_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_intersection_unsorted_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_intersection_unsorted_iterator);

        I2 it2;
        S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_intersection_unsorted_iterator(I it, S st, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> set_intersection_unsorted_iterator& {
            ++it;
            satisfy();
            return *this;
        }

    private:
        auto satisfy() -> void {
            while (it != st) {
                if (algorithms::contains(it2, st2, std::invoke(proj1, *it), proj2)) {
                    break;
                }
                ++it;
            }
        }
    };

    template <typename I, typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, operations::eq, Proj1, Proj2>
    struct set_symmetric_difference_unsorted_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_symmetric_difference_unsorted_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_symmetric_difference_unsorted_iterator);

        I2 it2;
        S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        bool from_first = true;
        I first_it;
        S first_st;

        GENEX_INLINE constexpr explicit set_symmetric_difference_unsorted_iterator(I it, S st, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), proj1(std::move(proj1)), proj2(std::move(proj2)), first_it(it), first_st(st) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it) and noexcept(*it2))
            -> value_type {
            return from_first ? *it : *it2;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)) and noexcept(std::addressof(*it2)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and noexcept(++it2))
            -> set_symmetric_difference_unsorted_iterator& {
            satisfy();
            return *this;
        }

    private:
        auto satisfy() -> void {
            while (it != st) {
                if (not algorithms::contains(it2, st2, std::invoke(proj1, *it), proj2)) {
                    from_first = true;
                    ++it;
                    return;
                }
                else {
                    ++it;
                }

                if (not algorithms::contains(first_it, first_st, std::invoke(proj2, *it2), proj1)) {
                    from_first = false;
                    ++it2;
                    return;
                }
                else {
                    ++it2;
                }
            }
        }
    };

    template <typename I, typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I, S, I2, S2, operations::eq, Proj1, Proj2>
    struct set_union_unsorted_iterator {
        using reference = std::common_reference_t<iter_reference_t<I>, iter_reference_t<I2>>;
        using value_type = std::common_type_t<iter_value_t<I>, iter_value_t<I2>>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(set_union_unsorted_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(set_union_unsorted_iterator);

        I2 it2;
        S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        bool from_first = true;
        I first_it;
        S first_st;

        GENEX_INLINE constexpr explicit set_union_unsorted_iterator(I it, S st, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it(std::move(it)), st(std::move(st)), it2(std::move(it2)), st2(std::move(st2)), proj1(std::move(proj1)), proj2(std::move(proj2)), first_it(it), first_st(st) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it) and noexcept(*it2))
            -> reference {
            return from_first ? *it : *it2;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)) and noexcept(std::addressof(*it2)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and noexcept(++it2))
            -> set_union_unsorted_iterator& {
            satisfy();
            return *this;
        }

    private:
        auto satisfy() -> void {
            while (it != st or it2 != st2) {
                if (it != st and (it2 == st2 or not algorithms::contains(first_it, first_st, std::invoke(proj2, *it2), proj1))) {
                    from_first = true;
                    ++it;
                    return;
                }
                else if (it2 != st2) {
                    from_first = false;
                    ++it2;
                    return;
                }
            }
        }
    };

    template <typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    struct set_difference_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_difference_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_difference_iterator, set_difference_sentinel, I2, S2, Comp, Proj1, Proj2);
    };

    template <typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    struct set_intersection_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_intersection_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_intersection_iterator, set_intersection_sentinel, I2, S2, Comp, Proj1, Proj2);
    };

    template <typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    struct set_symmetric_difference_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_symmetric_difference_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_symmetric_difference_iterator, set_symmetric_difference_sentinel, I2, S2, Comp, Proj1, Proj2);
    };

    template <typename S, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    struct set_union_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_union_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_union_iterator, set_union_sentinel, I2, S2, Comp, Proj1, Proj2);
    };

    template <typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    struct set_difference_unsorted_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_difference_unsorted_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_difference_unsorted_iterator, set_difference_unsorted_sentinel, I2, S2, Proj1, Proj2);
    };

    template <typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    struct set_intersection_unsorted_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_intersection_unsorted_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_intersection_unsorted_iterator, set_intersection_unsorted_sentinel, I2, S2, Proj1, Proj2);
    };

    template <typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    struct set_symmetric_difference_unsorted_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_symmetric_difference_unsorted_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_symmetric_difference_unsorted_iterator, set_symmetric_difference_unsorted_sentinel, I2, S2, Proj1, Proj2);
    };

    template <typename S, typename I2, typename S2, typename Proj1, typename Proj2>
    struct set_union_unsorted_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(set_union_unsorted_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(set_union_unsorted_iterator, set_union_unsorted_sentinel, I2, S2, Proj1, Proj2);
    };

    template <typename V, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, Comp, Proj1, Proj2>
    struct set_difference_view : std::ranges::view_interface<set_difference_view<V, V2, Comp, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_difference_iterator, set_difference_sentinel, iterator_t<V2>, sentinel_t<V2>, Comp, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_difference_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), comp, proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_difference_view(V rng, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };

    template <typename V, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, Comp, Proj1, Proj2>
    struct set_intersection_view : std::ranges::view_interface<set_intersection_view<V, V2, Comp, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_intersection_iterator, set_intersection_sentinel, iterator_t<V2>, sentinel_t<V2>, Comp, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_intersection_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), comp, proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_intersection_view(V rng, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };

    template <typename V, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, Comp, Proj1, Proj2>
    struct set_symmetric_difference_view : std::ranges::view_interface<set_symmetric_difference_view<V, V2, Comp, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_symmetric_difference_iterator, set_symmetric_difference_sentinel, iterator_t<V2>, sentinel_t<V2>, Comp, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_symmetric_difference_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), comp, proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_symmetric_difference_view(V rng, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };

    template <typename V, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, Comp, Proj1, Proj2>
    struct set_union_view : std::ranges::view_interface<set_union_view<V, V2, Comp, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_union_iterator, set_union_sentinel, iterator_t<V2>, sentinel_t<V2>, Comp, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_union_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), comp, proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_union_view(V rng, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };

    template <typename V, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, operations::eq, Proj1, Proj2>
    struct set_difference_unsorted_view : std::ranges::view_interface<set_difference_unsorted_view<V, V2, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_difference_unsorted_iterator, set_difference_unsorted_sentinel, iterator_t<V2>, sentinel_t<V2>, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_difference_unsorted_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_difference_unsorted_view(V rng, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };

    template <typename V, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, operations::eq, Proj1, Proj2>
    struct set_intersection_unsorted_view : std::ranges::view_interface<set_intersection_unsorted_view<V, V2, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_intersection_unsorted_iterator, set_intersection_unsorted_sentinel, iterator_t<V2>, sentinel_t<V2>, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_intersection_unsorted_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_intersection_unsorted_view(V rng, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };

    template <typename V, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, operations::eq, Proj1, Proj2>
    struct set_symmetric_difference_unsorted_view : std::ranges::view_interface<set_symmetric_difference_unsorted_view<V, V2, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_symmetric_difference_unsorted_iterator, set_symmetric_difference_unsorted_sentinel, iterator_t<V2>, sentinel_t<V2>, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_symmetric_difference_unsorted_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_symmetric_difference_unsorted_view(V rng, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };

    template <typename V, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V, V2, operations::eq, Proj1, Proj2>
    struct set_union_unsorted_view : std::ranges::view_interface<set_union_unsorted_view<V, V2, Proj1, Proj2>> {
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(set_union_unsorted_iterator, set_union_unsorted_sentinel, iterator_t<V2>, sentinel_t<V2>, Proj1, Proj2);
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(set_union_unsorted_view);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_union_unsorted_view(V rng, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng(std::move(rng)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    template <template <typename...> typename View>
    struct set_algorithm_sorted_base_fn {
    public:
        template <typename Rng1, typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires concepts::set_algorithmicable_range<Rng1, Rng2, Comp, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const -> auto {
            return View{
                std::views::all(std::forward<Rng1>(rng1)), std::views::all(std::forward<Rng2>(rng2)),
                std::move(comp), std::move(proj1), std::move(proj2)};
        }

        template <typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity> requires (not range<Comp>)
        constexpr auto operator()(Rng2 &&rng2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const -> auto {
            return std::bind_back(
                set_algorithm_sorted_base_fn{}, std::forward<Rng2>(rng2), std::move(comp), std::move(proj1), std::move(proj2));
        }
    };

    template <template <typename...> typename View>
    struct set_algorithm_unsorted_base_fn {
        template <typename Rng1, typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires concepts::set_algorithmicable_range<Rng1, Rng2, operations::eq, Proj1, Proj2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const -> auto {
            return View{
                std::views::all(std::forward<Rng1>(rng1)), std::views::all(std::forward<Rng2>(rng2)),
                std::move(proj1), std::move(proj2)};
        }

        template <typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires (not range<Proj1>)
        constexpr auto operator()(Rng2 &&rng2, Proj1 proj = {}, Proj2 proj2 = {}) const -> auto {
            return std::bind_back(
                set_algorithm_unsorted_base_fn{}, std::forward<Rng2>(rng2), std::move(proj), std::move(proj2));
        }
    };

    using set_difference_fn = set_algorithm_sorted_base_fn<detail::set_difference_view>;
    using set_intersection_fn = set_algorithm_sorted_base_fn<detail::set_intersection_view>;
    using set_symmetric_difference_fn = set_algorithm_sorted_base_fn<detail::set_symmetric_difference_view>;
    using set_union_fn = set_algorithm_sorted_base_fn<detail::set_union_view>;
    using set_difference_unsorted_fn = set_algorithm_unsorted_base_fn<detail::set_difference_unsorted_view>;
    using set_intersection_unsorted_fn = set_algorithm_unsorted_base_fn<detail::set_intersection_unsorted_view>;
    using set_symmetric_difference_unsorted_fn = set_algorithm_unsorted_base_fn<detail::set_symmetric_difference_unsorted_view>;
    using set_union_unsorted_fn = set_algorithm_unsorted_base_fn<detail::set_union_unsorted_view>;

    inline constexpr set_difference_fn set_difference{};
    inline constexpr set_intersection_fn set_intersection{};
    inline constexpr set_symmetric_difference_fn set_symmetric_difference{};
    inline constexpr set_union_fn set_union{};
    inline constexpr set_difference_unsorted_fn set_difference_unsorted{};
    inline constexpr set_intersection_unsorted_fn set_intersection_unsorted{};
    inline constexpr set_symmetric_difference_unsorted_fn set_symmetric_difference_unsorted{};
    inline constexpr set_union_unsorted_fn set_union_unsorted{};
}
