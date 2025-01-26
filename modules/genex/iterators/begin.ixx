module;
#include <iterator>

export module genex.iterators.begin;


namespace genex::iterators {
    export template <typename Rng>
    concept has_member_begin = requires(Rng &&r) { r.begin(); };

    export template <typename Rng>
    concept has_member_rbegin = requires(Rng &&r) { r.rbegin(); };

    export template <typename Rng>
    concept has_std_begin = requires(Rng &&r) { std::begin(r); };

    export template <typename Rng>
    concept has_std_rbegin = requires(Rng &&r) { std::rbegin(r); };

    export template <typename Rng>
    concept has_begin = has_member_begin<Rng> || has_std_begin<Rng>;

    export template <typename Rng>
    concept has_rbegin = has_member_rbegin<Rng> || has_std_rbegin<Rng>;

    struct begin_fn {
        template <typename Rng> requires has_member_begin<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.begin()) {
            return r.begin();
        }

        template <typename Rng> requires not has_member_begin<Rng> and has_std_begin<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::begin(r)) {
            return std::begin(r);
        }
    };

    struct rbegin_fn {
        template <typename Rng> requires has_member_rbegin<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.rbegin()) {
            return r.rbegin();
        }

        template <typename Rng> requires not has_member_rbegin<Rng> and has_std_rbegin<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::rbegin(r)) {
            return std::rbegin(r);
        }
    };

    export template <typename Rng>
    using begin_t = decltype(std::declval<begin_fn>()(std::declval<Rng>()));

    export template <typename Rng>
    using rbegin_t = decltype(std::declval<rbegin_fn>()(std::declval<Rng>()));

    export inline constexpr begin_fn begin;
    export inline constexpr rbegin_fn rbegin;
}
