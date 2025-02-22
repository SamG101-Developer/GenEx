module;
#include <iterator>

export module genex.iterators.distance;
import genex.iterators.begin;
import genex.iterators.end;
import genex.iterators.next;
import genex.concepts;


namespace genex::iterators {
    struct distance_fn {
        template <concepts::iterator I, concepts::sentinel S>
        constexpr auto operator()(I first, S last) const noexcept -> std::size_t {
            return std::distance(first, last);
        }

        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const noexcept -> std::size_t {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }
    };

    export inline constexpr distance_fn distance;
}
