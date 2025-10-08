#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/prev.hpp>


namespace genex::operations::detail::concepts {
    template <typename Rng>
    concept randomly_access_range =
        random_access_range<Rng>;

    template <typename Rng>
    concept frontable_range =
        input_range<Rng> and
        iterators::detail::concepts::beginable_range<Rng>;

    template <typename Rng>
    concept backable_range =
        bidirectional_range<Rng> and
        iterators::detail::concepts::endable_range<Rng>;
}


namespace genex::operations {
    struct at_fn {
        template <typename Rng>
        requires detail::concepts::randomly_access_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const std::size_t n) const -> range_value_t<Rng>& {
            return rng[n];
        }
    };

    struct front_fn {
        template <typename Rng>
        requires detail::concepts::frontable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> range_value_t<Rng> {
            return *iterators::begin(rng);
        }
    };

    struct back_fn {
        template <typename Rng>
        requires detail::concepts::backable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> range_value_t<Rng>& {
            return *iterators::prev(iterators::end(rng));
        }
    };

    inline constexpr at_fn at{};
    inline constexpr front_fn front{};
    inline constexpr back_fn back{};
}
