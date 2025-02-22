module;
#include <iterator>

export module genex.iterators.advance;
import genex.concepts;


namespace genex::iterators {
    export template <typename I>
    concept has_std_advance = requires(I &&it) { std::advance(std::forward<I>(it), 0); };

    export template <typename I>
    concept has_operator_plusplus = requires(I &&it) { ++it; };

    export template <typename I>
    concept has_advance = has_std_advance<I> || has_operator_plusplus<I>;

    struct advance_fn {
        template <concepts::iterator I> requires has_std_advance<I>
        constexpr auto operator()(I &&it, const std::size_t n = 1) const noexcept -> I {
            std::advance(it, n);
            return it;
        }

        template <concepts::iterator I> requires not has_std_advance<I> && has_operator_plusplus<I>
        constexpr auto operator()(I &&it, const std::size_t n = 1) const noexcept -> I {
            for (std::size_t i = 0; i < n; ++i) { ++it; }
            return it;
        }
    };

    export template <typename Rng>
    using advance_t = decltype(std::declval<advance_fn>()(std::declval<Rng>()));

    export inline constexpr advance_fn advance;
}
