#pragma once
#include <coroutine>
#include <genex/algorithms/contains.hpp>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    concept set_algorithmicable_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::indirectly_comparable<I1, I2, Comp, Proj1, Proj2>;

    template <typename Rng1, typename Rng2, typename Comp, typename Proj1, typename Proj2>
    concept set_algorithmicable_range =
        input_range<Rng1> and
        input_range<Rng2> and
        set_algorithmicable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Comp, Proj1, Proj2>;
}


namespace genex::views::detail {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_difference(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        if (first1 == last1) { co_return; }
        while (first1 != last1) {
            if (first2 == last2) {
                co_yield *first1;
                ++first1;
            }
            else if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) {
                co_yield *first1;
                ++first1;
            }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) {
                ++first2;
            }
            else {
                ++first1;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_intersection(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        if (first1 == last1) { co_return; }
        while (first1 != last1 and first2 != last2) {
            if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) {
                ++first1;
            }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) {
                ++first2;
            }
            else {
                co_yield *first1;
                ++first1;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_symmetric_difference(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        while (first1 != last1 or first2 != last2) {
            if (first1 == last1) {
                co_yield *first2;
                ++first2;
            }
            else if (first2 == last2) {
                co_yield *first1;
                ++first1;
            }
            else if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) {
                co_yield *first1;
                ++first1;
            }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) {
                co_yield *first2;
                ++first2;
            }
            else {
                ++first1;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_union(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        while (first1 != last1 or first2 != last2) {
            if (first1 == last1) {
                co_yield *first2;
                ++first2;
            }
            else if (first2 == last2) {
                co_yield *first1;
                ++first1;
            }
            else if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) {
                co_yield *first1;
                ++first1;
            }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) {
                co_yield *first2;
                ++first2;
            }
            else {
                co_yield *first1;
                ++first1;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
    auto do_set_difference_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (not algorithms::contains(first2, last2, std::invoke(std::forward<Proj>(proj), *first1))) { co_yield *first1; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
    auto do_set_intersection_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (algorithms::contains(first2, last2, std::invoke(std::forward<Proj>(proj), *first1))) { co_yield *first1; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    auto do_set_symmetric_difference_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        auto f1 = first1;
        auto f2 = first2;

        for (; first1 != last1; ++first1) {
            if (not algorithms::contains(f2, last2, *first1, std::forward<Proj2>(proj2))) { co_yield *first1; }
        }
        for (; first2 != last2; ++first2) {
            if (not algorithms::contains(f1, last1, *first2, std::forward<Proj1>(proj1))) { co_yield *first2; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj>
        requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
    auto do_set_union_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) -> generator<iter_value_t<I1>> {
        auto f1 = first1;
        for (; first1 != last1; ++first1) { co_yield *first1; }
        for (; first2 != last2; ++first2) {
            if (not algorithms::contains(f1, last1, *first2, std::forward<Proj>(proj))) { co_yield *first2; }
        }
    }
}


namespace genex::views {
    template <auto Func>
    struct set_algorithm_sorted_base_fn {
    public:
        template <typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            return std::invoke(
                Func, std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <typename Rng1, typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::set_algorithmicable_range<Rng1, Rng2, Comp, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return (*this)(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity> requires (not range<Comp>)
        constexpr auto operator()(Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            return std::bind_back(
                *this, std::forward<Rng2>(rng2), std::forward<Comp>(comp), std::forward<Proj1>(proj1),
                std::forward<Proj2>(proj2));
        }
    };

    template <auto Func>
    struct set_algorithm_unsorted_base_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
            requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
        constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj = {}) const -> auto {
            return std::invoke(
                Func, std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Proj>(proj));
        }

        template <typename Rng1, typename Rng2, typename Proj = meta::identity>
            requires concepts::set_algorithmicable_range<Rng1, Rng2, operations::eq, Proj, meta::identity>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return (*this)(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Proj>(proj));
        }

        template <typename Rng2, typename Proj = meta::identity>
            requires (not range<Proj>)
        constexpr auto operator()(Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            return std::bind_back(*this, std::forward<Rng2>(rng2), std::forward<Proj>(proj));
        }
    };

    struct set_difference_fn : set_algorithm_sorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
            (I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) {
                return detail::do_set_difference(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                    std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            }> {
    };

    struct set_intersection_fn : set_algorithm_sorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
            (I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) {
                return detail::do_set_intersection(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                    std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            }> {
    };

    struct set_symmetric_difference_fn : set_algorithm_sorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
            (I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1) {
                return detail::do_set_symmetric_difference(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                    std::forward<Comp>(comp), std::forward<Proj1>(proj1));
            }> {
    };

    struct set_union_fn : set_algorithm_sorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
            (I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) {
                return detail::do_set_union(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                    std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            }> {
    };

    struct set_difference_unsorted_fn : set_algorithm_unsorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
            (I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) {
                return detail::do_set_difference_unsorted(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Proj>(proj));
            }> {
    };

    struct set_intersection_unsorted_fn : set_algorithm_unsorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
            (I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) {
                return detail::do_set_intersection_unsorted(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Proj>(proj));
            }> {
    };

    struct set_symmetric_difference_unsorted_fn : set_algorithm_unsorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
            (I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) {
                return detail::do_set_symmetric_difference_unsorted(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                    std::forward<Proj>(proj), meta::identity{});
            }> {
    };

    struct set_union_unsorted_fn : set_algorithm_unsorted_base_fn<
            []<typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
                requires concepts::set_algorithmicable_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
            (I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) {
                return detail::do_set_union_unsorted(
                    std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Proj>(proj));
            }> {
    };

    EXPORT_GENEX_STRUCT(set_difference);
    EXPORT_GENEX_STRUCT(set_intersection);
    EXPORT_GENEX_STRUCT(set_symmetric_difference);
    EXPORT_GENEX_STRUCT(set_union);
    EXPORT_GENEX_STRUCT(set_difference_unsorted);
    EXPORT_GENEX_STRUCT(set_intersection_unsorted);
    EXPORT_GENEX_STRUCT(set_symmetric_difference_unsorted);
    EXPORT_GENEX_STRUCT(set_union_unsorted);
}
