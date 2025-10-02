#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Int>
    concept takeable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        integer_like<Int>;

    template <typename I, typename S, typename Int>
    concept takeable_iters_optimized_2 =
        std::random_access_iterator<I> and
        takeable_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept takeable_iters_optimized_1 =
        std::forward_iterator<I> and
        not std::random_access_iterator<I> and
        takeable_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept takeable_iters_unoptimized =
        std::input_iterator<I> and
        not std::forward_iterator<I> and
        takeable_iters<I, S, Int>;

    template <typename Rng, typename Int>
    concept can_take_range =
        input_range<Rng> and
        takeable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
        requires concepts::takeable_iters_optimized_2<I, S, Int>
    GENEX_NO_ASAN
    auto do_take(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last or n == 0) { co_return; }
        const auto size = std::distance(first, last);
        const auto m = n > size ? size : n;
        for (auto i = 0; i < m; ++i, ++first) {
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::takeable_iters_optimized_1<I, S, Int>
    GENEX_NO_ASAN
    auto do_take(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last or n == 0) { co_return; }
        auto limit = std::next(first, n, last);
        for (; first != limit; ++first) {
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::takeable_iters_unoptimized<I, S, Int>
    GENEX_NO_ASAN
    auto do_take(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last or n == 0) { co_return; }
        for (auto m = n; first != last and m > 0; ++first, --m) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    struct take_fn {
        template <typename I, typename S, typename Int>
            requires concepts::takeable_iters<I, S, Int>
        constexpr auto operator()(I first, S last, const Int n) const -> auto {
            return detail::do_take(std::move(first), std::move(last), n);
        }

        template <typename Rng, typename Int>
            requires concepts::can_take_range<Rng, Int>
        constexpr auto operator()(Rng &&rng, const Int n) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), n);
        }

        template <typename Int>
            requires integer_like<Int>
        constexpr auto operator()(const Int n) const -> auto {
            return std::bind_back(take_fn{}, n);
        }
    };

    GENEX_EXPORT_STRUCT(take);
}
