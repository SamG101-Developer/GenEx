#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::iterators::detail::concepts {
    template <typename I, typename S>
    concept distancable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        requires(I first, S last) { std::distance(std::move(first), std::move(last)); };

    template <typename Rng>
    concept distancable_range =
        input_range<Rng> and
        distancable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::iterators {
    struct distance_fn {
        template <typename I, typename S>
        requires detail::concepts::distancable_iters<I, S>
        GENEX_INLINE auto operator()(I first, S last) const -> std::size_t {
            return std::distance(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::distancable_range<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const -> std::size_t {
            auto [first, last] = iter_pair(rng);
            return std::distance(std::move(first), std::move(last));
        }
    };

    inline constexpr distance_fn distance{};
}
