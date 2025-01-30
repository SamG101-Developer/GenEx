module;
#include <iterator>

export module genex.iterators.next;


namespace genex::iterators {
    template <typename Rng>
    concept has_std_next = requires(Rng &&r) { std::next(r); };

    template <typename Rng>
    concept has_std_advance = requires(Rng &&r) { std::advance(r, std::declval<size_t>()); };

    template <typename Rng>
    concept has_operator_plusplus = requires(Rng &&r) { ++r; };

    export template <typename Rng>
    concept has_next = has_std_next<Rng> || has_std_advance<Rng> || has_operator_plusplus<Rng>;

    struct next_fn {
        template <typename Rng> requires has_std_next<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::next(r)) {
            return std::next(r);
        }

        template <typename Rng> requires not has_std_next<Rng> && has_std_advance<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> Rng {
            std::advance(r, 1);
            return r;
        }

        template <typename Rng> requires not has_std_next<Rng> && not has_std_advance<Rng> && has_operator_plusplus<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> Rng {
            (void)++r;
            return r;
        }
    };

    export template <typename Rng>
    using next_t = decltype(std::declval<next_fn>()(std::declval<Rng>()));

    export inline constexpr next_fn next;
}
