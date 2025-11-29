#pragma once
#include <genex/span.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/next.hpp>


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept droppable_range =
        input_range<Rng> and
        droppable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views2 {
    struct drop_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_iters<I, S, Int> and std::random_access_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            return genex::span<iter_value_t<I>>(first + n, last);
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            // todo: slow
            return std::ranges::subrange(iterators::next(std::move(first), n), std::move(last));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_range<Rng, Int> and std::random_access_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return genex::span<range_value_t<Rng>>(first + n, last);
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            // todo: slow
            auto [first, last] = iterators::iter_pair(rng);
            return std::ranges::subrange(iterators::next(std::move(first), n), std::move(last));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            return meta::bind_back(drop_fn{}, std::move(n));
        }
    };

    inline constexpr drop_fn drop{};
}
