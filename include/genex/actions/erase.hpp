#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng, typename I, typename S>
    concept erasable_range =
        range<Rng> and
        std::sentinel_for<S, I> and
        std::same_as<std::remove_cvref_t<I>, iterator_t<Rng>> and
        std::same_as<std::remove_cvref_t<S>, sentinel_t<Rng>> and
        has_member_erase<Rng>;
}


namespace genex::actions {
    struct erase_fn final {
        template <typename Rng, typename I, typename S>
        requires detail::concepts::erasable_range<Rng, I, S>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, I it, S end) const -> decltype(auto) {
            return rng.erase(std::move(it), std::move(end));
        }

        template <typename Rng, typename I>
        requires detail::concepts::erasable_range<Rng, I, sentinel_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, I it) const {
            return rng.erase(std::move(it));
        }
    };

    inline constexpr erase_fn erase{};
}
