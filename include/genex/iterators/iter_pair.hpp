#pragma once
#include <genex/iterators/access.hpp>


namespace genex::iterators::concepts {
    template <typename Rng>
    concept can_iter_pair_range =
        range<Rng> and
        beginable_range<Rng> and
        endable_range<Rng>;
}


namespace genex::iterators {
    struct iter_pair_fn {
        template <typename Rng>
            requires concepts::can_iter_pair_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return std::make_pair(iterators::begin(rng), iterators::end(rng));
        }
    };

    GENEX_EXPORT_STRUCT(iter_pair);
}
