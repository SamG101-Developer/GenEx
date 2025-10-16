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
    concept can_slice_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept can_slice_range =
        input_range<Rng> and
        can_slice_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S, typename Int>
    requires concepts::can_slice_iters<I, S, Int> and std::random_access_iterator<I>
    auto do_slice(I first, S last, const Int start_index, const Int end_index, const Int step = 1) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        auto true_last = first + end_index;
        for (auto i = start_index; i < end_index; i += step) {
            co_yield *(first + i);
        }
    }

    template <typename I, typename S, typename Int>
    requires concepts::can_slice_iters<I, S, Int> and std::bidirectional_iterator<I>
    auto do_slice(I first, S last, const Int start_index, const Int end_index, const Int step = 1) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        auto it = iterators::prev(iterators::distance(first, last), end_index, first);
        for (auto i = start_index; i < end_index and (first + i) != last; i += step) {
            co_yield *it;
            it = iterators::next(it, step, last);
        }
    }

    template <typename I, typename S, typename Int>
    requires concepts::can_slice_iters<I, S, Int>
    auto do_slice(I first, S last, const Int start_index, const Int end_index, const Int step = 1) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (auto i = 0u; i < start_index and first != last; ++i, ++first) { }
        for (auto i = start_index; i < end_index and first != last; i += step) {
            co_yield *first;
            for (auto j = 0u; j < step and first != last; ++j, ++first);
        }
    }
}


namespace genex::views {
    struct slice_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::can_slice_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, Int start_index, Int end_index, Int step = static_cast<Int>(1)) const {
            return detail::impl::do_slice(std::move(first), std::move(last), std::move(start_index), std::move(end_index), std::move(step));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::can_slice_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int start_index, Int end_index, Int step = static_cast<Int>(1)) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_slice(std::move(first), std::move(last), std::move(start_index), std::move(end_index), std::move(step));
        }

        template <typename Int>
        requires integer_like<Int>
        GENEX_INLINE constexpr auto operator()(Int start_index, Int end_index, Int step = 1) const {
            return meta::bind_back(slice_fn{}, start_index, end_index, step);
        }
    };

    inline constexpr slice_fn slice{};
}
