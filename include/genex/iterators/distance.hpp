#pragma once
#include <iterator>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>


namespace genex::iterators::concepts {
    template <typename I, typename S>
    concept can_distance_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        requires(I first, S last) { std::distance(std::move(first), std::move(last)); };

    template <typename Rng>
    concept can_distance_range =
        input_range<Rng> and
        can_distance_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::iterators {
    DEFINE_ITERATOR(distance) {
        template <typename I, typename S> requires concepts::can_distance_iters<I, S>
        constexpr auto operator()(I first, S last) const noexcept -> std::size_t {
            return std::distance(std::move(first), std::move(last));
        }

        template <typename Rng> requires concepts::can_distance_range<Rng>
        constexpr auto operator()(Rng &&rng) const noexcept -> std::size_t {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }
    };

    EXPORT_GENEX_ITERATOR(distance);
}
