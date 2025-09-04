#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/prev.hpp>


namespace genex::operations::concepts {
    template <typename Rng>
    concept randomly_access_range =
        random_access_range<Rng>;

    template <typename Rng>
    concept frontable_range =
        forward_range<Rng> and
        iterators::concepts::beginable_range<Rng>;

    template <typename Rng>
    concept backable_range =
        bidirectional_range<Rng> and
        iterators::concepts::endable_range<Rng>;
}


namespace genex::operations {
    struct at_fn {
        template <typename Rng>
            requires concepts::randomly_access_range<Rng>
        constexpr auto operator()(Rng &&rng, const std::size_t n) const -> range_value_t<Rng>& {
            return rng[n];
        }
    };

    struct front_fn {
        template <typename Rng>
            requires concepts::frontable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> range_value_t<Rng> {
            return *iterators::begin(rng);
        }
    };

    struct back_fn {
        template <typename Rng>
            requires concepts::backable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> range_value_t<Rng>& {
            return *iterators::prev(iterators::end(rng));
        }
    };

    EXPORT_GENEX_STRUCT(at);
    EXPORT_GENEX_STRUCT(front);
    EXPORT_GENEX_STRUCT(back);
}
