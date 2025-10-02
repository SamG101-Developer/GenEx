#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/next.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Int>
    concept can_slice_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        integer_like<Int>;

    template <typename I, typename S, typename Int>
    concept can_slice_iters_optimized_2 =
        std::random_access_iterator<I> and
        can_slice_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept can_slice_iters_optimized_1 =
        std::forward_iterator<I> and
        not std::random_access_iterator<I> and
        can_slice_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept can_slice_iters_unoptimized =
        std::input_iterator<I> and
        not std::forward_iterator<I> and
        can_slice_iters<I, S, Int>;

    template <typename Rng, typename Int>
    concept can_slice_range =
        input_range<Rng> and
        can_slice_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
        requires concepts::can_slice_iters_optimized_2<I, S, Int>
    GENEX_NO_ASAN
    auto do_slice(I first, S last, const Int start_index, const Int end_index, const Int step = 1) -> generator<iter_value_t<I>> {
        if (step <= 0) { throw std::invalid_argument("Step cannot be <= 0."); }
        if (first == last) { co_return; }
        for (auto i = start_index; i < end_index and (first + i) != last; i += step) {
            co_yield *(first + i);
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::can_slice_iters_optimized_1<I, S, Int>
    GENEX_NO_ASAN
    auto do_slice(I first, S last, const Int start_index, const Int end_index, const Int step = 1) -> generator<iter_value_t<I>> {
        if (step <= 0) { throw std::invalid_argument("Step cannot be <= 0."); }
        if (first == last) { co_return; }
        auto it = iterators::next(first, start_index, last);
        for (auto i = start_index; i < end_index and (first + i) != last; i += step) {
            co_yield *it;
            it = iterators::next(it, step, last);
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::can_slice_iters_unoptimized<I, S, Int>
    GENEX_NO_ASAN
    auto do_slice(I first, S last, const Int start_index, const Int end_index, const Int step = 1) -> generator<iter_value_t<I>> {
        if (step <= 0) { throw std::invalid_argument("Step cannot be <= 0."); }
        if (first == last) { co_return; }
        for (auto i = 0u; i < start_index and first != last; ++i, ++first) {
        }
        for (auto i = start_index; i < end_index and first != last; i += step) {
            co_yield *first;
            for (auto j = 0u; j < step and first != last; ++j, ++first);
        }
    }
}


namespace genex::views {
    struct slice_fn {
        template <typename I, typename S, typename Int>
            requires concepts::can_slice_iters<I, S, Int>
        constexpr auto operator()(I first, S last, const Int start_index, const Int end_index, const Int step = 1) const -> auto {
            return detail::do_slice(std::move(first), std::move(last), start_index, end_index, step);
        }

        template <typename Rng, typename Int>
            requires concepts::can_slice_range<Rng, Int>
        constexpr auto operator()(Rng &&rng, const Int start_index, const Int end_index, const Int step = 1) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), start_index, end_index, step);
        }

        template <typename Int>
            requires integer_like<Int>
        constexpr auto operator()(const Int start_index, const Int end_index, const Int step = 1) const -> auto {
            return std::bind_back(slice_fn{}, start_index, end_index, step);
        }
    };

    GENEX_EXPORT_STRUCT(slice);
}
