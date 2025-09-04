#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/next.hpp>
#include <genex/iterators/prev.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Int>
    concept takeable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        integer_like<Int>;

    template <typename I, typename S, typename Int>
    concept takeable_last_iters_optimized_3 =
        std::random_access_iterator<I> and
        takeable_last_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept takeable_last_iters_optimized_2 =
        std::bidirectional_iterator<I> and
        not std::random_access_iterator<I> and
        takeable_last_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept takeable_last_iters_optimized_1 =
        std::forward_iterator<I> and
        not std::bidirectional_iterator<I> and
        takeable_last_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept takeable_last_iters_unoptimized =
        std::input_iterator<I> and
        not std::forward_iterator<I> and
        takeable_last_iters<I, S, Int>;

    template <typename Rng, typename Int>
    concept takeable_last_range =
        input_range<Rng> and
        takeable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
        requires concepts::takeable_last_iters_optimized_3<I, S, Int>
    auto do_take_last(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        // Use the distance to optimize random access ranges.
        if (first == last or n == 0) { co_return; }
        const auto size = iterators::distance(first, last);
        first = iterators::next(first, size > n ? size - n : 0, last);
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::takeable_last_iters_optimized_2<I, S, Int>
    auto do_take_last(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        // Use the prev iterator to optimize bidirectional ranges.
        if (first == last or n == 0) { co_return; }
        auto limit = iterators::prev(last, n, first);
        for (; limit != last; ++limit) {
            co_yield static_cast<iter_value_t<I>>(*limit);
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::takeable_last_iters_optimized_1<I, S, Int>
    auto do_take_last(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        // Use the n-steps-ahead iterator to optimize forward ranges.
        if (first == last or n == 0) { co_return; }
        auto limit = iterators::next(first, n, last);
        for (; limit != last; ++limit) {
            co_yield static_cast<iter_value_t<I>>(*limit);
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::takeable_last_iters_unoptimized<I, S, Int>
    auto do_take_last(I first, S last, Int n) -> generator<iter_value_t<I>> {
        // Fallback for input iterators: count the distance and increment.
        if (first == last or n == 0) { co_return; }
        const auto size = iterators::distance(first, last);
        n = size > n ? n : size - n;
        auto i = 0uz;
        for (; first != last; ++first) {
            if (i++ < size - n) { continue; }
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct take_last_fn {
        template <typename I, typename S, typename Int>
            requires concepts::takeable_last_iters<I, S, Int>
        constexpr auto operator()(I first, S last, const Int n) const -> auto {
            return detail::do_take_last(
                std::move(first), std::move(last), n);
        }

        template <typename Rng, typename Int>
            requires concepts::takeable_last_range<Rng, Int>
        constexpr auto operator()(Rng &&rng, const Int n) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), n);
        }

        template <typename Int>
            requires integer_like<Int>
        constexpr auto operator()(const Int n) const -> auto {
            return std::bind_back(take_last_fn{}, n);
        }
    };

    EXPORT_GENEX_STRUCT(take_last);
}
