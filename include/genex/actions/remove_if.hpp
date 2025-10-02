#pragma once
#include <functional>
#include <utility>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/algorithms/find_if.hpp>
#include <genex/iterators/access.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename Pred, typename Proj>
    concept removable_if_range =
        forward_range<Rng> and
        erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>> and
        std::permutable<iterator_t<Rng>> and
        std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;
}


namespace genex::actions {
    struct remove_if_fn {
        template <typename Rng, typename Pred, typename Proj = meta::identity>
            requires concepts::removable_if_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            // todo: optimize to prevent multi-passes.
            while (true) {
                auto it = algorithms::find_if(rng, std::forward<Pred>(pred), std::forward<Proj>(proj));
                if (it == iterators::end(rng)) { break; }
                actions::erase(rng, std::move(it));
            }
            return std::forward<Rng>(rng);
        }

        template <typename Pred, typename Proj = meta::identity>
            requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            return std::bind_back(remove_if_fn{}, std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(remove_if);
}
