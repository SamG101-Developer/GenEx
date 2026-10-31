module;
#include <genex/detail/macros.hpp>

export module genex.actions.sort;
export import genex.pipe;
import genex.algorithms.sort;
import genex.algorithms.stable_sort;
import genex.concepts;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Comp, typename Proj>
  concept sortable_range =
    random_access_range<Rng> and
    sortable<iterator_t<Rng>, Comp, Proj>;
}

namespace genex::actions {
  struct sort_fn : pipe_closure {
    template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
    requires detail::concepts::sortable_range<Rng, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const -> Rng {
      genex::sort(rng, std::move(comp), std::move(proj));
      return std::forward<Rng>(rng);
    }

    template <typename Comp = operations::lt, typename Proj = meta::identity>
    requires closure_callable<Comp>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(Comp comp, Proj proj = {}) const {
      return meta::bind_back(sort_fn{}, std::move(comp), std::move(proj));
    }
  };

  struct stable_sort_fn : pipe_closure {
    template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
    requires detail::concepts::sortable_range<Rng, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const -> Rng {
      genex::stable_sort(rng, std::move(comp), std::move(proj));
      return std::forward<Rng>(rng);
    }

    template <typename Comp = operations::lt, typename Proj = meta::identity>
    requires closure_callable<Comp>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(Comp comp, Proj proj = {}) const {
      return meta::bind_back(stable_sort_fn{}, std::move(comp), std::move(proj));
    }
  };

  export inline constexpr sort_fn sort{};
  export inline constexpr stable_sort_fn stable_sort{};
}
