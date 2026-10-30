module;
#include <genex/detail/macros.hpp>

export module genex.actions.remove;
export import genex.pipe;
import genex.actions.erase;
import genex.concepts;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename E, typename Proj>
  concept removable_range =
    forward_range<Rng> and
    std::permutable<iterator_t<Rng>> and
    std::indirect_equivalence_relation<
      operations::eq,
      std::projected<iterator_t<Rng>, Proj>,
      std::remove_cvref_t<E> const *>;
}

namespace genex::actions {
  struct remove_fn {
    template <typename Rng, typename E, typename Proj = meta::identity>
      requires detail::concepts::removable_range<Rng, E, Proj>
    constexpr auto operator()(Rng &&rng, E elem, Proj proj = {}) const -> Rng {
      detail::keep_if(rng, [&]<typename T>(T &&x) -> bool {
        return not (meta::invoke(proj, std::forward<T>(x)) == elem);
      });
      return std::forward<Rng>(rng);
    }

    template <typename E, typename Proj = meta::identity>
      requires (not range<E>)
    GENEX_INLINE constexpr auto operator()(E &&elem, Proj &&proj = {}) const {
      return meta::bind_back(remove_fn{}, std::move(elem), std::move(proj));
    }
  };

  export inline constexpr remove_fn remove{};
}
