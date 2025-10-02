#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::iterators::concepts {
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
            requires concepts::distancable_iters<I, S>
        auto operator()(I first, S last) const noexcept -> std::size_t {
            return std::distance(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::distancable_range<Rng>
        auto operator()(Rng &&rng) const noexcept -> std::size_t {
            auto [first, last] = iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last));
        }
    };

    GENEX_EXPORT_STRUCT(distance);
}
