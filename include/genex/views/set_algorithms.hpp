#pragma once
#include <coroutine>
#include <genex/algorithms/contains.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


// unsorted -> uses "contains" algorithm
// sorted -> uses merge algorithm (not implemented yet)
// todo: projections?


namespace genex::views::detail {
    template <typename I1, typename S1, typename I2, typename S2>
    auto do_set_difference_unsorted(I1 first1, S1 last1, I2 first2, S2 last2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (!algorithms::contains(first2, last2, *first1)) { co_yield *first1; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2>
    auto do_set_intersection_unsorted(I1 first1, S1 last1, I2 first2, S2 last2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (algorithms::contains(first2, last2, *first1)) { co_yield *first1; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2>
    auto do_set_symmetric_difference_unsorted(I1 first1, S1 last1, I2 first2, S2 last2) -> generator<iter_value_t<I1>> {
        auto f1 = first1;
        auto f2 = first2;

        for (; first1 != last1; ++first1) {
            if (not algorithms::contains(f2, last2, *first1)) { co_yield *first1; }
        }
        for (; first2 != last2; ++first2) {
            if (not algorithms::contains(f1, last1, *first2)) { co_yield *first2; }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2>
    auto do_set_union_unsorted(I1 first1, S1 last1, I2 first2, S2 last2) -> generator<iter_value_t<I1>> {
        auto f1 = first1;
        for (; first1 != last1; ++first1) { co_yield *first1; }
        for (; first2 != last2; ++first2) {
            if (not algorithms::contains(f1, last1, *first2)) { co_yield *first2; }
        }
    }
}


namespace genex::views {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp>
    concept can_set_alg_sorted_iters =
        input_iterator<I1> and
        input_iterator<I2> and
        sentinel_for<S1, I1> and
        sentinel_for<S2, I2> and
        std::indirect_strict_weak_order<Comp, I1, I2>;

    template <typename Rng1, typename Rng2, typename Comp>
    concept can_set_alg_sorted_range =
        input_range<Rng1> and
        input_range<Rng2> and
        std::indirect_strict_weak_order<Comp, iterator_t<Rng1>, iterator_t<Rng2>>;

    template <typename I1, typename S1, typename I2, typename S2>
    concept can_set_alg_unsorted_iters =
        input_iterator<I1> and
        forward_iterator<I2> and
        sentinel_for<S1, I1> and
        sentinel_for<S2, I2> and
        std::equality_comparable_with<iter_value_t<I1>, iter_value_t<I2>>;

    template <typename Rng1, typename Rng2>
    concept can_set_alg_unsorted_range =
        input_range<Rng1> and
        forward_range<Rng2> and
        std::equality_comparable_with<range_value_t<Rng1>, range_value_t<Rng2>>;

    DEFINE_VIEW(set_difference_unsorted) {
        template <typename I1, typename S1, typename I2, typename S2> requires can_set_alg_unsorted_iters<I1, S1, I2, S2>
        constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2) const -> auto {
            auto gen = detail::do_set_difference_unsorted(std::move(first1), std::move(last1), std::move(first2), std::move(last2));
            return set_difference_unsorted_view(std::move(gen));
        }

        template <typename Rng1, typename Rng2> requires can_set_alg_unsorted_range<Rng1, Rng2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            return (*this)(iterators::begin(rng1), iterators::end(rng1), iterators::begin(rng2), iterators::end(rng2));
        }

        template <typename Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires can_set_alg_unsorted_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    DEFINE_VIEW(set_intersection_unsorted) {
        template <typename I1, typename S1, typename I2, typename S2> requires can_set_alg_unsorted_iters<I1, S1, I2, S2>
        constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2) const -> auto {
            auto gen = detail::do_set_intersection_unsorted(std::move(first1), std::move(last1), std::move(first2), std::move(last2));
            return set_intersection_unsorted_view(std::move(gen));
        }

        template <typename Rng1, typename Rng2> requires can_set_alg_unsorted_range<Rng1, Rng2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            return (*this)(iterators::begin(rng1), iterators::end(rng1), iterators::begin(rng2), iterators::end(rng2));
        }

        template <typename Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires can_set_alg_unsorted_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    DEFINE_VIEW(set_symmetric_difference_unsorted) {
        template <typename I1, typename S1, typename I2, typename S2> requires can_set_alg_unsorted_iters<I1, S1, I2, S2>
        constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2) const -> auto {
            auto gen = detail::do_set_symmetric_difference_unsorted(std::move(first1), std::move(last1), std::move(first2), std::move(last2));
            return set_intersection_unsorted_view(std::move(gen));
        }

        template <typename Rng1, typename Rng2> requires can_set_alg_unsorted_range<Rng1, Rng2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            return (*this)(iterators::begin(rng1), iterators::end(rng1), iterators::begin(rng2), iterators::end(rng2));
        }

        template <typename Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires can_set_alg_unsorted_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    DEFINE_VIEW(set_union_unsorted) {
        template <typename I1, typename S1, typename I2, typename S2> requires can_set_alg_unsorted_iters<I1, S1, I2, S2>
        constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2) const -> auto {
            auto gen = detail::do_set_union_unsorted(std::move(first1), std::move(last1), std::move(first2), std::move(last2));
            return set_intersection_unsorted_view(std::move(gen));
        }

        template <typename Rng1, typename Rng2> requires can_set_alg_unsorted_range<Rng1, Rng2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            return (*this)(iterators::begin(rng1), iterators::end(rng1), iterators::begin(rng2), iterators::end(rng2));
        }

        template <typename Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires can_set_alg_unsorted_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    EXPORT_GENEX_VIEW(set_difference_unsorted);
    EXPORT_GENEX_VIEW(set_intersection_unsorted);
    EXPORT_GENEX_VIEW(set_symmetric_difference_unsorted);
    EXPORT_GENEX_VIEW(set_union_unsorted);
}
