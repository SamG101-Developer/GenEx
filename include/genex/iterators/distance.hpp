#pragma once
#include <iterator>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::concepts;


namespace genex::iterators {
    struct distance_fn {
        template <iterator I, sentinel S>
        constexpr auto operator()(I first, S last) const noexcept -> std::size_t {
            return std::distance(first, last);
        }

        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const noexcept -> std::size_t {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }
    };

    EXPORT_GENEX_STRUCT(distance);
}
