#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/next.hpp>
#include <genex/iterators/prev.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_last_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept droppable_last_range =
        forward_range<Rng> and
        droppable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S, typename Int>
    requires concepts::droppable_last_iters<I, S, Int> and std::random_access_iterator<I>
    auto do_drop_last(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        auto true_last = first + (iterators::distance(first, last) - n);
        for (; first != true_last; ++first) {
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }

    template <typename I, typename S, typename Int>
    requires concepts::droppable_last_iters<I, S, Int> and std::bidirectional_iterator<I>
    auto do_drop_last(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        const auto true_last = iterators::prev(last, n, first);
        for (; first != true_last; ++first) {
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }

    template <typename I, typename S, typename Int>
    requires concepts::droppable_last_iters<I, S, Int>
    auto do_drop_last(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        auto true_last = iterators::next(first, n, last);
        for (; first != true_last; ++first) {
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct drop_last_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_last_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, Int n) const {
            return detail::coros::do_drop_last(std::move(first), std::move(last), std::move(n));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_last_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_drop_last(std::move(first), std::move(last), std::move(n));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int n) const {
            return std::bind_back(drop_last_fn{}, std::move(n));
        }
    };

    inline constexpr drop_last_fn drop_last{};
}
