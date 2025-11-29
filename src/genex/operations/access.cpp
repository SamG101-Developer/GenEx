module;
#include <genex/macros.hpp>

export module genex.operations.access;
import genex.concepts;
import genex.iterators.access;
import genex.iterators.prev;
import std;


namespace genex::operations::detail::concepts {
    template <typename Rng>
    concept randomly_access_range =
        random_access_range<Rng>;

    template <typename Rng>
    concept frontable_range =
        input_range<Rng>;

    template <typename Rng>
    concept backable_range =
        bidirectional_range<Rng>;
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

    export inline constexpr at_fn at{};
    export inline constexpr front_fn front{};
    export inline constexpr back_fn back{};
}
