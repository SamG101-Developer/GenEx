module;
#include <genex/macros.hpp>

export module genex.iterators.iter_pair;
import genex.concepts;
import genex.iterators.access;
import std;


namespace genex::iterators::detail::concepts {
    template <typename Rng>
    concept can_iter_pair_range =
        input_range<Rng>;
}


namespace genex::iterators {
    struct iter_pair_fn {
        template <typename Rng>
        requires detail::concepts::can_iter_pair_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            return std::make_pair(iterators::begin(rng), iterators::end(rng));
        }
    };

    export inline constexpr iter_pair_fn iter_pair{};
}
