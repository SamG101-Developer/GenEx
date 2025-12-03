module;
#include <genex/macros.hpp>

export module genex.actions.sort;
export import genex.pipe;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Comp, typename Proj>
    concept can_sorted_range =
        random_access_range<Rng> and
        std::sortable<iterator_t<Rng>, Comp, Proj>;
}


namespace genex::actions {
    struct sort_fn {
        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
        requires detail::concepts::can_sorted_range<Rng, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            auto sorter = [&]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
                return meta::invoke(comp, meta::invoke(proj, std::forward<Lhs>(lhs)), meta::invoke(proj, std::forward<Rhs>(rhs)));
            };
            std::sort(std::move(first), std::move(last), std::move(sorter));
            return std::forward<Rng>(rng);
        }

        template <typename Comp = operations::lt, typename Proj = meta::identity>
        requires (not range<Comp>)
        GENEX_INLINE constexpr auto operator()(Comp comp = {}, Proj proj = {}) const {
            return meta::bind_back(sort_fn{}, std::move(comp), std::move(proj));
        }
    };

    export inline constexpr sort_fn sort{};
}
