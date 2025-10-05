#pragma once
#include <genex/iterators/access.hpp>


namespace genex::iterators::concepts {
    template <typename Rng>
    concept can_iter_pair_range =
        range<Rng>;
}


namespace genex::iterators {
    struct iter_pair_fn {
        template <typename Rng>
        requires concepts::can_iter_pair_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            return std::make_pair(iterators::begin(rng), iterators::end(rng));
        }
    };

    inline constexpr iter_pair_fn iter_pair{};
}
