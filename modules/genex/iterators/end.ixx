module;
#include <iterator>

export module genex.iterators.end;


namespace genex::iterators {
    export template <typename Rng>
    concept has_member_end = requires(Rng &&r) { r.end(); };

    export template <typename Rng>
    concept has_member_rend = requires(Rng &&r) { r.rend(); };

    export template <typename Rng>
    concept has_std_end = requires(Rng &&r) { std::end(r); };

    export template <typename Rng>
    concept has_std_rend = requires(Rng &&r) { std::rend(r); };

    export template <typename Rng>
    concept has_end = has_member_end<Rng> || has_std_end<Rng>;

    export template <typename Rng>
    concept has_rend = has_member_rend<Rng> || has_std_rend<Rng>;

    struct end_fn {
        template <typename Rng> requires iterators::has_member_end<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.end());

        template <typename Rng> requires not iterators::has_member_end<Rng> and iterators::has_std_end<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::end(r));
    };

    struct rend_fn {
        template <typename Rng> requires iterators::has_member_rend<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.rend());

        template <typename Rng> requires not iterators::has_member_rend<Rng> and iterators::has_std_rend<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::rend(r));
    };

    export template <typename Rng>
    using end_t = decltype(std::declval<end_fn>()(std::declval<Rng>()));

    export template <typename Rng>
    using rend_t = decltype(std::declval<rend_fn>()(std::declval<Rng>()));

    export inline constexpr end_fn end;
    export inline constexpr rend_fn rend;
}


template <typename Rng> requires genex::iterators::has_member_end<Rng>
constexpr auto genex::iterators::end_fn::operator()(Rng &&r) const noexcept -> decltype(r.end()) {
    return r.end();
}

template <typename Rng> requires not genex::iterators::has_member_end<Rng> and genex::iterators::has_std_end<Rng>
constexpr auto genex::iterators::end_fn::operator()(Rng &&r) const noexcept -> decltype(std::end(r)) {
    return std::end(r);
}

template <typename Rng> requires genex::iterators::has_member_rend<Rng>
constexpr auto genex::iterators::rend_fn::operator()(Rng &&r) const noexcept -> decltype(r.rend()) {
    return r.rend();
}

template <typename Rng> requires not genex::iterators::has_member_rend<Rng> and genex::iterators::has_std_rend<Rng>
constexpr auto genex::iterators::rend_fn::operator()(Rng &&r) const noexcept -> decltype(std::rend(r)) {
    return std::rend(r);
}
