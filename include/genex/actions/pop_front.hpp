#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/iterators/access.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng>
    concept front_poppable_range =
        range<Rng>;

    template <typename Rng>
    concept front_poppable_select_pop_front =
        front_poppable_range<Rng> and
        has_member_pop_front<Rng>;

    template <typename Rng>
    concept front_poppable_select_erase =
        front_poppable_range<Rng> and
        erasable_range<Rng, iterator_t<Rng>, iterator_t<Rng>> and
        has_member_erase<Rng> and
        not front_poppable_select_pop_front<Rng>;
}


namespace genex::actions {
    struct pop_front_fn {
        template <typename Rng>
        requires detail::concepts::front_poppable_select_pop_front<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            return rng.pop_front();
        }

        template <typename Rng>
        requires detail::concepts::front_poppable_select_erase<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const -> decltype(auto) {
            return actions::erase(rng, iterators::begin(rng));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(pop_front_fn{});
        }
    };

    inline constexpr pop_front_fn pop_front{};
}
