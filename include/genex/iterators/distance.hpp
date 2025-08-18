#pragma once
#include <iterator>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::concepts;


namespace genex::iterators {
    DEFINE_ITERATOR(distance) {
        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const noexcept -> std::size_t {
            return std::distance(std::forward<I>(first), std::forward<S>(last));
        }

        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const noexcept -> std::size_t {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }
    };

    EXPORT_GENEX_ITERATOR(distance);
}
