#pragma once
#include <iterator>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>


namespace genex::iterators {
    template <typename I, typename S>
    concept can_distance_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_distance_range =
        input_range<Rng>;

    DEFINE_ITERATOR(distance) {
        template <typename I, typename S> requires can_distance_iters<I, S>
        constexpr auto operator()(I first, S last) const noexcept -> std::size_t {
            return std::distance(std::move(first), std::move(last));
        }

        template <typename Rng> requires can_distance_range<Rng>
        constexpr auto operator()(Rng &&rng) const noexcept -> std::size_t {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }
    };

    EXPORT_GENEX_ITERATOR(distance);
}
