module;
#include <genex/macros.hpp>

export module genex.actions.unique;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.actions.erase;
import genex.iterators.access;
import genex.operations.cmp;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Comp, typename Proj>
  concept uniqueable_range =
    forward_range<Rng> and
    has_member_erase<Rng> and
    std::indirectly_comparable<iterator_t<Rng>, iterator_t<Rng>, Comp, Proj, Proj>;
}

namespace genex::actions {
  struct unique_fn {
    template <typename Rng, typename Comp = operations::eq, typename Proj = meta::identity>
      requires detail::concepts::uniqueable_range<Rng, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const -> decltype(auto) {
      auto it = iterators::begin(rng);
      if (it == iterators::end(rng)) { return std::forward<Rng>(rng); }

      auto prev = it;
      ++it;
      while (it != iterators::end(rng)) {
        if (meta::invoke(comp, meta::invoke(proj, *prev), meta::invoke(proj, *it))) {
          it = actions::erase(rng, it);
        }
        else {
          prev = it;
          ++it;
        }
      }
      return std::forward<Rng>(rng);
    }

    template <typename Comp = operations::eq, typename Proj = meta::identity>
      requires (not range<Comp>)
    GENEX_INLINE constexpr auto operator()(Comp comp = {}, Proj proj = {}) const {
      return meta::bind_back(unique_fn{}, std::move(comp), std::move(proj));
    }
  };

  export inline constexpr unique_fn unique{};
}