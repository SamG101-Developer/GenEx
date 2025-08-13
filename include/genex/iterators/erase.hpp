#pragma once
#include <iterator>


namespace genex::iterators {
    template <typename Rng>
    concept has_member_erase = requires(Rng &&r, begin_t<Rng>&& it) { r.erase(std::forward<begin_t<Rng>>(it)); };

    template <typename Rng>
    concept has_std_erase = requires(Rng &&r, begin_t<Rng>&& it) { std::erase(std::forward<Rng>(r), std::forward<begin_t<Rng>>(it)); };

    template <typename Rng>
    concept has_erase = has_member_erase<Rng> || has_std_erase<Rng>;

    struct erase_fn {
        template <typename Rng> requires (has_member_erase<Rng>)
        constexpr auto operator()(Rng &&r, begin_t<Rng>&& it) const noexcept -> decltype(r.erase(std::forward<begin_t<Rng>>(it))) {
            return r.erase(std::forward<begin_t<Rng>>(it));
        }

        template <typename Rng> requires (not has_member_erase<Rng> and has_std_erase<Rng>)
        constexpr auto operator()(Rng &&r, begin_t<Rng>&& it) const noexcept -> decltype(std::erase(std::forward<Rng>(r))) {
            return std::erase(std::forward<Rng>(r), std::forward<begin_t<Rng>>(it));
        }
    };

    inline constexpr erase_fn erase;
}
