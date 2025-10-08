#pragma once
#include <genex/iterators/access.hpp>


namespace genex::iterators::detail::concepts {
    template <typename Rng>
    concept can_iter_pair_range =
        range<Rng> and
        beginable_range<Rng> and
        endable_range<Rng>;
}


namespace genex::iterators {
    struct iter_pair_fn {
        template <typename Rng>
        requires detail::concepts::can_iter_pair_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            return std::make_pair(iterators::begin(rng), iterators::end(rng));
        }
    };

    inline constexpr iter_pair_fn iter_pair{};
}
