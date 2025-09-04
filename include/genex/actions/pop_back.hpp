#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/erase.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/prev.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::concepts {
    template <typename Rng>
    concept back_poppable_range =
        range<Rng>;

    template <typename Rng>
    concept back_poppable_select_pop_back =
        back_poppable_range<Rng> and
        has_member_pop_back<Rng>;

    template <typename Rng>
    concept back_poppable_select_erase =
        back_poppable_range<Rng> and
        erasable_range<Rng, iterator_t<Rng>, iterator_t<Rng>> and
        has_member_erase<Rng> and
        not back_poppable_select_pop_back<Rng>;
}


namespace genex::actions {
    struct pop_back_fn {
        template <typename Rng>
            requires concepts::back_poppable_select_pop_back<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return rng.pop_back();
        }

        template <typename Rng>
            requires concepts::back_poppable_select_erase<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return actions::erase(rng, iterators::prev(iterators::end(rng)));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(pop_back_fn{});
        }
    };

    EXPORT_GENEX_STRUCT(pop_back);
}
