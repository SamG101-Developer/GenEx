module;
#include <genex/macros.hpp>

export module genex.actions.remove_if;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.actions.erase;
import genex.algorithms.find_if;
import genex.iterators.access;
import std;


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Pred, typename Proj>
    concept removable_if_range =
        forward_range<Rng> and
        std::permutable<iterator_t<Rng>> and
        std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;
}


namespace genex::actions {
    struct remove_if_fn {
        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::removable_if_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const -> decltype(auto) {
            // todo: optimize to prevent multi-passes.
            while (true) {
                auto it = genex::find_if(rng, pred, proj);
                if (it == iterators::end(rng)) { break; }
                actions::erase(rng, std::move(it));
            }
            return std::forward<Rng>(rng);
        }

        template <typename Pred, typename Proj = meta::identity>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
            return meta::bind_back(remove_if_fn{}, std::move(pred), std::move(proj));
        }
    };

    export inline constexpr remove_if_fn remove_if{};
}
