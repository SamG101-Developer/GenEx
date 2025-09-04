#pragma once
#include <utility>
#include <genex/actions/erase.hpp>
#include <genex/algorithms/find.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename E, typename Proj>
    concept removable_range =
        forward_range<Rng> and
        erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>> and
        std::permutable<iterator_t<Rng>> and
        std::indirect_equivalence_relation<operations::eq, std::projected<iterator_t<Rng>, Proj>, const E*>;
}


namespace genex::actions {
    struct remove_fn {
        template <typename Rng, typename E, typename Proj = meta::identity>
            requires concepts::removable_range<Rng, E, Proj>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            while (true) {
                auto it = algorithms::find(rng, std::forward<E>(elem), std::forward<Proj>(proj));
                if (it == iterators::end(rng)) { break; }
                actions::erase(rng, std::move(it));
            }
            return std::forward<Rng>(rng);
        }

        template <typename E>
            requires (not input_range<std::remove_cvref_t<E>>)
        constexpr auto operator()(E &&elem) const -> auto {
            return std::bind_back(remove_fn{}, std::forward<E>(elem));
        }
    };

    EXPORT_GENEX_STRUCT(remove);
}
