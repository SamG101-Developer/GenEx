#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/prev.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng>
    concept back_poppable_range =
        range<Rng>;

    template <typename Rng>
    concept back_poppable_select_pop_back =
        back_poppable_range<Rng> and
        has_member_pop_back<Rng>;

    template <typename Rng>
    concept back_poppable_select_erase =
        bidirectional_range<Rng> and
        back_poppable_range<Rng> and
        erasable_range<Rng, iterator_t<Rng>, iterator_t<Rng>> and
        has_member_erase<Rng> and
        not back_poppable_select_pop_back<Rng>;
}


namespace genex::actions {
    struct pop_back_fn {
        template <typename Rng>
        requires detail::concepts::back_poppable_select_pop_back<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            return rng.pop_back();
        }

        template <typename Rng>
        requires detail::concepts::back_poppable_select_erase<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const -> decltype(auto) {
            return actions::erase(rng, iterators::prev(iterators::end(rng)));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(pop_back_fn{});
        }
    };

    inline constexpr pop_back_fn pop_back{};
}
