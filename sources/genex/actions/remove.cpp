module;
#include <genex/detail/macros.hpp>

export module genex.actions.remove;
export import genex.pipe;
import genex.concepts;
import genex.detail.keep_if;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename E, typename Proj>
  concept removable_range =
    forward_range<Rng> and
    permutable<iterator_t<Rng>> and
    indirect_equivalence_relation<
      operations::eq,
      projected<iterator_t<Rng>, Proj>,
      std::remove_cvref_t<E> const *>;
}

namespace genex::actions {
  struct remove_fn : pipe_closure {
    template <typename Rng, typename E, typename Proj = meta::identity>
    requires detail::concepts::removable_range<Rng, E, Proj>
    constexpr auto operator()(Rng &&rng, E elem, Proj proj = {}) const -> Rng {
      detail::keep_if(rng, [&]<typename T>(T &&x) -> bool {
        return not (meta::invoke(proj, std::forward<T>(x)) == elem);
      });
      return std::forward<Rng>(rng);
    }

    template <typename E, typename Proj = meta::identity>
    requires closure_value<E>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(E &&elem, Proj proj = {}) const {
      return meta::bind_back(remove_fn{}, std::forward<E>(elem), std::move(proj));
    }
  };

  export inline constexpr remove_fn remove{};
}
