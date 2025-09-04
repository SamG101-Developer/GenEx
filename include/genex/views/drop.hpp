#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/advance.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        integer_like<Int>;

    template <typename I, typename S, typename Int>
    concept droppable_iters_optimized_2 =
        std::random_access_iterator<I> and
        droppable_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept droppable_iters_optimized_1 =
        std::forward_iterator<I> and
        not std::random_access_iterator<I> and
        droppable_iters<I, S, Int>;

    template <typename I, typename S, typename Int>
    concept droppable_iters_unoptimized =
        std::input_iterator<I> and
        not std::forward_iterator<I> and
        droppable_iters<I, S, Int>;

    template <typename Rng, typename Int>
    concept droppable_range =
        input_range<Rng> and
        droppable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
        requires concepts::droppable_iters_optimized_2<I, S, Int>
    auto do_drop(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        const auto size = iterators::distance(first, last);
        if (n >= size) { co_return; }
        iterators::advance(first, n);
        for (; first != last; ++first) {
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::droppable_iters_optimized_1<I, S, Int>
    auto do_drop(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        first = iterators::next(first, n, last);
        for (; first != last; ++first) {
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Int>
        requires concepts::droppable_iters_unoptimized<I, S, Int>
    auto do_drop(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        for (auto i = 0u; i < n and first != last; ++i, ++first) {
        }
        for (; first != last; ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    struct drop_fn {
        template <typename I, typename S, typename Int>
            requires concepts::droppable_iters<I, S, Int>
        auto operator()(I first, S last, const Int n) const -> auto {
            return detail::do_drop(
                std::move(first), std::move(last), n);
        }

        template <typename Rng, typename Int>
            requires concepts::droppable_range<Rng, Int>
        auto operator()(Rng &&rng, const Int n) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), n);
        }

        template <typename Int>
            requires integer_like<Int>
        auto operator()(Int n) const -> auto {
            return std::bind_back(drop_fn{}, n);
        }
    };

    EXPORT_GENEX_STRUCT(drop);
}
