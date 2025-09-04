#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/erase.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/prev.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::concepts {
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
        template <typename Rng> requires concepts::front_poppable_select_pop_front<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return rng.pop_front();
        }

        template <typename Rng> requires concepts::front_poppable_select_erase<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return actions::erase(rng, iterators::begin(rng));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_front(pop_front_fn{});
        }
    };

    EXPORT_GENEX_STRUCT(pop_front);
}
