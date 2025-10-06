#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/algorithms/contains.hpp>
#include <genex/iterators/access.hpp>
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
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    struct set_difference_iterator {
        using reference = iter_reference_t<I1>;
        using value_type = iter_value_t<I1>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_difference_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_difference_iterator, it1);

        GENEX_INLINE constexpr explicit set_difference_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1)) -> reference {
            return *it1;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_difference_iterator& {
            ++it1;
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
        noexcept(it1++)) -> set_difference_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() const -> void {
            while (it1 != st1) {
                // Advance it2 until either it2 == st2 or greater then the current value.
                while (it2 != st2 and std::invoke(comp, std::invoke(proj2, *it2), std::invoke(proj1, *it1))) {
                    ++it2;
                }

                // If we reached the end of it2 or the current value is less than the current value of it2, we are done.
                if (it2 == st2 or std::invoke(comp, std::invoke(proj1, *it1), std::invoke(proj2, *it2))) {
                    break;
                }

                // Otherwise, the current value is equal to the current value of it2, so we need to advance it.
                if (it2 != st2 and not std::invoke(comp, std::invoke(proj1, *it1), std::invoke(proj2, *it2)) and not std::invoke(comp, std::invoke(proj2, *it2), std::invoke(proj1, *it1))) {
                    ++it1;
                }
            }
        }
    };

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    struct set_intersection_iterator {
        using reference = iter_reference_t<I1>;
        using value_type = iter_value_t<I1>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_intersection_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_intersection_iterator, it1);

        GENEX_INLINE constexpr explicit set_intersection_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1)) -> reference {
            return *it1;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_intersection_iterator& {
            ++it1;
            if (it1 != st1) {
                ++it2;
                satisfy();
            }
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it1++)) -> set_intersection_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it1 != st1 and it2 != st2) {
                auto &&v1 = std::invoke(proj1, *it1);
                auto &&v2 = std::invoke(proj2, *it2);

                if (std::invoke(comp, v1, v2)) {
                    ++it1;
                }
                else if (std::invoke(comp, v1, v2)) {
                    ++it2;
                }
                else {
                    break;
                }
            }

            if (it1 == st1 or it2 == st2) {
                it1 = st1;
            }
        }
    };


    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    struct set_symmetric_difference_iterator {
        using reference = iter_reference_t<I1>;
        using value_type = iter_value_t<I1>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;
        bool from_first = true;

        GENEX_INLINE constexpr explicit set_symmetric_difference_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_symmetric_difference_iterator, it1);

        GENEX_INLINE constexpr explicit set_symmetric_difference_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1) and noexcept(*it2)) -> reference {
            return from_first ? *it1 : *it2;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_symmetric_difference_iterator& {
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it1++)) -> set_symmetric_difference_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it1 != st1 and it2 != st2) {
                if (it1 == st1) {
                    from_first = false;
                    ++it2;
                    return;
                }
                if (it2 == st2) {
                    from_first = true;
                    ++it1;
                    return;
                }

                auto &&v1 = std::invoke(proj1, *it1);
                auto &&v2 = std::invoke(proj2, *it2);

                if (std::invoke(comp, v1, v2)) {
                    from_first = true;
                    ++it1;
                    return;
                }
                else if (std::invoke(comp, v2, v1)) {
                    from_first = false;
                    ++it2;
                    return;
                }
                else {
                    ++it1;
                    ++it2;
                }
            }

            if (it1 == st1 and it2 != st2) {
                it1 = st1;
            }
        }
    };


    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    struct set_union_iterator {
        using reference = iter_reference_t<I1>;
        using value_type = iter_value_t<I1>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;
        bool from_first = true;

        GENEX_INLINE constexpr explicit set_union_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_union_iterator, it1);

        GENEX_INLINE constexpr explicit set_union_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)), comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1) and noexcept(*it2)) -> reference {
            return from_first ? *it1 : *it2;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_union_iterator& {
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it1++)) -> set_union_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it1 != st1 or it2 != st2) {
                auto &&v1 = std::invoke(proj1, *it1);
                auto &&v2 = std::invoke(proj2, *it2);

                if (std::invoke(comp, v1, v2)) {
                    from_first = true;
                    ++it1;
                    return;
                }
                else if (std::invoke(comp, v2, v1)) {
                    from_first = false;
                    ++it2;
                    return;
                }
                else {
                    from_first = true;
                    ++it1;
                    ++it2;
                    return;
                }
            }
        }
    };

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    struct set_difference_unsorted_iterator {
        using reference = iter_reference_t<I1>;
        using value_type = iter_value_t<I1>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_difference_unsorted_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_difference_unsorted_iterator, it1);

        GENEX_INLINE constexpr explicit set_difference_unsorted_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)),
            proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1)) -> reference {
            return *it1;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_difference_unsorted_iterator& {
            ++it1;
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it1++)) -> set_difference_unsorted_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it1 != st1) {
                if (not algorithms::contains(it2, st2, std::invoke(proj1, *it1), proj2)) {
                    break;
                }
                ++it1;
            }
        }
    };

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    struct set_intersection_unsorted_iterator {
        using reference = iter_reference_t<I1>;
        using value_type = iter_value_t<I1>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_INLINE constexpr explicit set_intersection_unsorted_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_intersection_unsorted_iterator, it1);

        GENEX_INLINE constexpr explicit set_intersection_unsorted_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)),
            proj1(std::move(proj1)), proj2(std::move(proj2)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1)) -> reference {
            return *it1;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_intersection_unsorted_iterator& {
            ++it1;
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it1++)) -> set_intersection_unsorted_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            while (it1 != st1) {
                if (algorithms::contains(it2, st2, std::invoke(proj1, *it1), proj2)) {
                    break;
                }
                ++it1;
            }
        }
    };

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    struct set_symmetric_difference_unsorted_iterator {
        using reference = iter_reference_t<I1>;
        using value_type = iter_value_t<I1>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        bool from_first = true;
        I1 first_it;
        S1 first_st;

        GENEX_INLINE constexpr explicit set_symmetric_difference_unsorted_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_symmetric_difference_unsorted_iterator, it1);

        GENEX_INLINE constexpr explicit set_symmetric_difference_unsorted_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)),
            proj1(std::move(proj1)), proj2(std::move(proj2)), first_it(it1), first_st(st1) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1) and noexcept(*it2)) -> reference {
            return from_first ? *it1 : *it2;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_symmetric_difference_unsorted_iterator& {
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it1++)) -> set_symmetric_difference_unsorted_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            ++(from_first ? it1 : it2);
            while (it1 != st1 or it2 != st2) {
                if (it1 != st1 and not algorithms::contains(it2, st2, std::invoke(proj1, *it1), proj2)) {
                    from_first = true;
                    return;
                }

                if (it2 != st2 and not algorithms::contains(first_it, first_st, std::invoke(proj2, *it2), proj1)) {
                    from_first = false;
                    return;
                }

                if (it1 != st1) { ++it1; }
                if (it2 != st2) { ++it2; }
            }
        }
    };

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    struct set_union_unsorted_iterator {
        using reference = std::common_reference_t<iter_reference_t<I1>, iter_reference_t<I2>>;
        using value_type = std::common_type_t<iter_value_t<I1>, iter_value_t<I2>>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I1>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I1>;

        I1 it1; S1 st1;
        I2 it2; S2 st2;
        Proj1 proj1;
        Proj2 proj2;

        bool from_first = true;
        I1 first_it;
        S1 first_st;

        GENEX_INLINE constexpr explicit set_union_unsorted_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            set_union_unsorted_iterator, it1);

        GENEX_INLINE constexpr explicit set_union_unsorted_iterator(I1 it1, S1 st1, I2 it2, S2 st2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<I1> and
            std::is_nothrow_move_constructible_v<S1> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            it1(std::move(it1)), st1(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)),
            proj1(std::move(proj1)), proj2(std::move(proj2)), first_it(it1), first_st(st1) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it1) and noexcept(*it2)) -> reference {
            return from_first ? *it1 : *it2;
        }


        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(satisfy())) -> set_union_unsorted_iterator& {
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it1++)) -> set_union_unsorted_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() -> void {
            ++(from_first ? it1 : it2);
            while (true) {
                if (it1 != st1) {
                    from_first = true;
                    return;
                }
                while (it2 != st2 && algorithms::contains(first_it, first_st, std::invoke(proj2, *it2), proj1)) {
                    ++it2;
                }
                if (it2 != st2) {
                    from_first = false;
                    return;
                }
                break;
            }
        }
    };

    struct set_difference_sentinel { };

    struct set_intersection_sentinel { };

    struct set_symmetric_difference_sentinel { };

    struct set_union_sentinel { };

    struct set_difference_unsorted_sentinel { };

    struct set_intersection_unsorted_sentinel { };

    struct set_symmetric_difference_unsorted_sentinel { };

    struct set_union_unsorted_sentinel { };

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    set_difference_iterator(I1, S1, I2, S2, Comp, Proj1, Proj2) -> set_difference_iterator<I1, S1, I2, S2, Comp, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    set_intersection_iterator(I1, S1, I2, S2, Comp, Proj1, Proj2) -> set_intersection_iterator<I1, S1, I2, S2, Comp, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    set_symmetric_difference_iterator(I1, S1, I2, S2, Comp, Proj1, Proj2) -> set_symmetric_difference_iterator<I1, S1, I2, S2, Comp, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    set_union_iterator(I1, S1, I2, S2, Comp, Proj1, Proj2) -> set_union_iterator<I1, S1, I2, S2, Comp, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    set_difference_unsorted_iterator(I1, S1, I2, S2, Proj1, Proj2) -> set_difference_unsorted_iterator<I1, S1, I2, S2, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    set_intersection_unsorted_iterator(I1, S1, I2, S2, Proj1, Proj2) -> set_intersection_unsorted_iterator<I1, S1, I2, S2, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    set_symmetric_difference_unsorted_iterator(I1, S1, I2, S2, Proj1, Proj2) -> set_symmetric_difference_unsorted_iterator<I1, S1, I2, S2, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    set_union_unsorted_iterator(I1, S1, I2, S2, Proj1, Proj2) -> set_union_unsorted_iterator<I1, S1, I2, S2, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_difference_iterator, set_difference_sentinel, I1, S1, I2, S2, Comp, Proj1, Proj2) {
        return it.it1 == it.st1;
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_intersection_iterator, set_intersection_sentinel, I1, S1, I2, S2, Comp, Proj1, Proj2) {
        return it.it1 == it.st1;
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_symmetric_difference_iterator, set_symmetric_difference_sentinel, I1, S1, I2, S2, Comp, Proj1, Proj2) {
        return it.it1 == it.st1 and it.it2 == it.st2;
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_union_iterator, set_union_sentinel, I1, S1, I2, S2, Comp, Proj1, Proj2) {
        return it.it1 == it.st1 and it.it2 == it.st2;
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_difference_unsorted_iterator, set_difference_unsorted_sentinel, I1, S1, I2, S2, Proj1, Proj2) {
        return it.it1 == it.st1;
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_intersection_unsorted_iterator, set_intersection_unsorted_sentinel, I1, S1, I2, S2, Proj1, Proj2) {
        return it.it1 == it.st1;
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_symmetric_difference_unsorted_iterator, set_symmetric_difference_unsorted_sentinel, I1, S1, I2, S2, Proj1, Proj2) {
        return it.it1 == it.st1 and it.it2 == it.st2;
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(set_union_unsorted_iterator, set_union_unsorted_sentinel, I1, S1, I2, S2, Proj1, Proj2) {
        return it.it1 == it.st1 and it.it2 == it.st2;
    }

    template <typename V1, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, Comp, Proj1, Proj2>
    struct set_difference_view : std::ranges::view_interface<set_difference_view<V1, V2, Comp, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_difference_iterator, set_difference_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2),
            comp, proj1, proj2);


        GENEX_INLINE constexpr explicit set_difference_view(V1 rng1, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)),
            comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };

    template <typename V1, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, Comp, Proj1, Proj2>
    struct set_intersection_view : std::ranges::view_interface<set_intersection_view<V1, V2, Comp, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_intersection_iterator, set_intersection_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2),
            comp, proj1, proj2);

        GENEX_INLINE constexpr explicit set_intersection_view(V1 rng1, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)),
            comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };

    template <typename V1, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, Comp, Proj1, Proj2>
    struct set_symmetric_difference_view : std::ranges::view_interface<set_symmetric_difference_view<V1, V2, Comp, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_symmetric_difference_iterator, set_symmetric_difference_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2), comp, proj1, proj2);

        GENEX_INLINE constexpr explicit set_symmetric_difference_view(V1 rng1, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)),
            comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };

    template <typename V1, typename V2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, Comp, Proj1, Proj2>
    struct set_union_view : std::ranges::view_interface<set_union_view<V1, V2, Comp, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Comp comp;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_union_iterator, set_union_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2), comp, proj1, proj2);

        GENEX_INLINE constexpr explicit set_union_view(V1 rng1, V2 rng2, Comp comp, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Comp> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)),
            comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };

    template <typename V1, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, operations::eq, Proj1, Proj2>
    struct set_difference_unsorted_view : std::ranges::view_interface<set_difference_unsorted_view<V1, V2, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_difference_unsorted_iterator, set_difference_unsorted_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);

        GENEX_INLINE constexpr explicit set_difference_unsorted_view(V1 rng1, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };

    template <typename V1, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, operations::eq, Proj1, Proj2>
    struct set_intersection_unsorted_view : std::ranges::view_interface<set_intersection_unsorted_view<V1, V2, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_intersection_unsorted_iterator, set_intersection_unsorted_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);

        GENEX_INLINE constexpr explicit set_intersection_unsorted_view(V1 rng1, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };

    template <typename V1, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, operations::eq, Proj1, Proj2>
    struct set_symmetric_difference_unsorted_view : std::ranges::view_interface<set_symmetric_difference_unsorted_view<V1, V2, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_symmetric_difference_unsorted_iterator, set_symmetric_difference_unsorted_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);

        GENEX_INLINE constexpr explicit set_symmetric_difference_unsorted_view(V1 rng1, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };

    template <typename V1, typename V2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_range<V1, V2, operations::eq, Proj1, Proj2>
    struct set_union_unsorted_view : std::ranges::view_interface<set_union_unsorted_view<V1, V2, Proj1, Proj2>> {
        V1 base_rng1;
        V2 base_rng2;
        Proj1 proj1;
        Proj2 proj2;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            set_union_unsorted_iterator, set_union_unsorted_sentinel, base_rng1,
            iterators::begin(base_rng2), iterators::end(base_rng2), proj1, proj2);

        GENEX_INLINE constexpr explicit set_union_unsorted_view(V1 rng1, V2 rng2, Proj1 proj1, Proj2 proj2) noexcept(
            std::is_nothrow_move_constructible_v<V1> and
            std::is_nothrow_move_constructible_v<V2> and
            std::is_nothrow_move_constructible_v<Proj1> and
            std::is_nothrow_move_constructible_v<Proj2>) :
            base_rng1(std::move(rng1)), base_rng2(std::move(rng2)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng1))) {
            return iterators::begin(base_rng1);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng1))) {
            return iterators::end(base_rng1);
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<V1>, range_size_t<V2>> = delete;
    };
}


namespace genex::views {
    template <template <typename...> typename View>
    struct set_algorithm_sorted_base_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        constexpr auto operator()(I1 it1, S1 st1, I2 it2, S2 st2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const noexcept -> auto {
            using V1 = std::ranges::subrange<I1, S1>;
            using V2 = std::ranges::subrange<I2, S2>;
            return View<V1, V2>{
                std::ranges::subrange{std::move(it1), std::move(st1)}, std::ranges::subrange{std::move(it2), std::move(st2)},
                std::move(comp), std::move(proj1), std::move(proj2)};
        }

        template <typename Rng1, typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires concepts::set_algorithmicable_range<Rng1, Rng2, Comp, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const noexcept -> auto {
            using V1 = std::views::all_t<Rng1>;
            using V2 = std::views::all_t<Rng2>;
            return View<V1, V2>{
                std::forward<Rng1>(rng1), std::forward<Rng2>(rng2),
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
        template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
        constexpr auto operator()(I1 it1, S1 st1, I2 it2, S2 st2, Proj1 proj1 = {}, Proj2 proj2 = {}) const -> auto {
            using V1 = std::ranges::subrange<I1, S1>;
            using V2 = std::ranges::subrange<I2, S2>;
            return View<V1, V2, Proj1, Proj2>{
                std::ranges::subrange{std::move(it1), std::move(st1)}, std::ranges::subrange{std::move(it2), std::move(st2)},
                std::move(proj1), std::move(proj2)};
        }

        template <typename Rng1, typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires concepts::set_algorithmicable_range<Rng1, Rng2, operations::eq, Proj1, Proj2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const -> auto {
            using V1 = std::views::all_t<Rng1>;
            using V2 = std::views::all_t<Rng2>;
            return View<V1, V2, Proj1, Proj2>{
                std::forward<Rng1>(rng1), std::forward<Rng2>(rng2),
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
