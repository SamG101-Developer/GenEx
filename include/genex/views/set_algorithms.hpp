#pragma once
#include <coroutine>
#include <genex/algorithms/contains.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/views/_view_base.hpp>


// Todo: set algorithm base class to prevent all the repeated code being written.


namespace genex::views::concepts {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    concept can_set_algorithm_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::movable<I1> and
        std::movable<I2> and
        std::indirectly_comparable<I1, I2, Comp, Proj1, Proj2>;

    template <typename Rng1, typename Rng2, typename Comp, typename Proj1, typename Proj2>
    concept can_set_algorithm_range =
        input_range<Rng1> and
        input_range<Rng2> and
        can_set_algorithm_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Comp, Proj1, Proj2>;
}


namespace genex::views::detail {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_difference(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        if (first1 == last1) { co_return; }
        while (first1 != last1) {
            if (first2 == last2) { co_yield *first1++; }
            else if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) { co_yield *first1++; }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) { ++first2; }
            else {
                ++first1;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_intersection(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        if (first1 == last1) { co_return; }
        while (first1 != last1 and first2 != last2) {
            if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) { ++first1; }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) { ++first2; }
            else {
                co_yield *first1++;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_symmetric_difference(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        while (first1 != last1 or first2 != last2) {
            if (first1 == last1) { co_yield *first2++; }
            else if (first2 == last2) { co_yield *first1++; }
            else if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) { co_yield *first1++; }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) { co_yield *first2++; }
            else {
                ++first1;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    auto do_set_union(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        while (first1 != last1 or first2 != last2) {
            if (first1 == last1) { co_yield *first2++; }
            else if (first2 == last2) { co_yield *first1++; }
            else if (std::invoke(comp, std::invoke(proj1, *first1), std::invoke(proj2, *first2))) { co_yield *first1++; }
            else if (std::invoke(comp, std::invoke(proj2, *first2), std::invoke(proj1, *first1))) { co_yield *first2++; }
            else {
                co_yield *first1++;
                ++first2;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, meta::identity, Proj>
    auto do_set_difference_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (!algorithms::contains(first2, last2, *first1, std::forward<Proj>(proj))) { co_yield *first1; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, meta::identity, Proj>
    auto do_set_intersection_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (algorithms::contains(first2, last2, *first1, std::forward<Proj>(proj))) { co_yield *first1; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
    auto do_set_symmetric_difference_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 &&proj, Proj2 &&proj2) -> generator<iter_value_t<I1>> {
        auto f1 = first1;
        auto f2 = first2;

        for (; first1 != last1; ++first1) {
            if (not algorithms::contains(f2, last2, *first1, std::forward<Proj1>(proj))) { co_yield *first1; }
        }
        for (; first2 != last2; ++first2) {
            if (not algorithms::contains(f1, last1, *first2, std::forward<Proj2>(proj2))) { co_yield *first2; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj> requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, meta::identity, Proj>
    auto do_set_union_unsorted(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj) -> generator<iter_value_t<I1>> {
        auto f1 = first1;
        for (; first1 != last1; ++first1) { co_yield *first1; }
        for (; first2 != last2; ++first2) {
            if (not algorithms::contains(f1, last1, *first2, std::forward<Proj>(proj))) { co_yield *first2; }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(set_difference) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_difference(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            return set_difference_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <
            typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        auto operator()(Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, comp, proj1, proj2)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            };
        }
    };

    DEFINE_VIEW(set_intersection) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_intersection(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            return set_intersection_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <
            typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        auto operator()(Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, comp, proj1, proj2)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            };
        }
    };

    DEFINE_VIEW(set_symmetric_difference) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_symmetric_difference(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            return set_symmetric_difference_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <
            typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        auto operator()(Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, comp, proj1, proj2)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            };
        }
    };

    DEFINE_VIEW(set_union) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_union(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            return set_union_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <
            typename Rng2, typename Comp = operations::eq,
            typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        auto operator()(Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, comp, proj1, proj2)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, Comp, Proj1, Proj2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            };
        }
    };

    DEFINE_VIEW(set_difference_unsorted) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_difference_unsorted(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Proj>(proj));
            return set_difference_unsorted_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Proj = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj, meta::identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Proj>(proj));
        }

        template <typename Rng2, typename Proj = meta::identity>
        auto operator()(Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, proj)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj, meta::identity>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Proj>(proj));
            };
        }
    };

    DEFINE_VIEW(set_intersection_unsorted) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_intersection_unsorted(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Proj>(proj));
            return set_intersection_unsorted_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Proj = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj, meta::identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Proj>(proj));
        }

        template <typename Rng2, typename Proj = meta::identity>
        auto operator()(Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, proj)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj, meta::identity>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Proj>(proj));
            };
        }
    };

    DEFINE_VIEW(set_symmetric_difference_unsorted) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, Proj1, Proj2>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_symmetric_difference_unsorted(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2),
                std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            return set_symmetric_difference_unsorted_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj1, Proj2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        auto operator()(Rng2 &&rng2, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, proj1, proj2)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj1, Proj2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
            };
        }
    };

    DEFINE_VIEW(set_union_unsorted) {
        template <
            typename I1, typename S1, typename I2, typename S2, typename Proj = meta::identity>
            requires concepts::can_set_algorithm_iters<I1, S1, I2, S2, operations::eq, Proj, meta::identity>
        auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj &&proj = {}) const -> auto {
            // Call the set algorithm inner function.
            auto gen = detail::do_set_union_unsorted(
                std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Proj>(proj));
            return set_union_unsorted_view(std::move(gen));
        }

        template <
            typename Rng1, typename Rng2, typename Proj = meta::identity>
            requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj, meta::identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            // Call the set algorithm inner function.
            return (*this)(
                iterators::begin(std::forward<Rng1>(rng1)), iterators::end(std::forward<Rng1>(rng1)),
                iterators::begin(std::forward<Rng2>(rng2)), iterators::end(std::forward<Rng2>(rng2)),
                std::forward<Proj>(proj));
        }

        template <typename Rng2, typename Proj = meta::identity>
        auto operator()(Rng2 &&rng2, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the set algorithm.
            return
                [FWD_CAPTURES(rng2, proj)]<typename Rng1> requires concepts::can_set_algorithm_range<Rng1, Rng2, operations::eq, Proj, meta::identity>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(set_difference);
    EXPORT_GENEX_VIEW(set_intersection);
    EXPORT_GENEX_VIEW(set_symmetric_difference);
    EXPORT_GENEX_VIEW(set_union);
    EXPORT_GENEX_VIEW(set_difference_unsorted);
    EXPORT_GENEX_VIEW(set_intersection_unsorted);
    EXPORT_GENEX_VIEW(set_symmetric_difference_unsorted);
    EXPORT_GENEX_VIEW(set_union_unsorted);
}
