module;
#include <genex/detail/macros.hpp>

export module genex.actions.remove_if;
export import genex.pipe;
import genex.concepts;
import genex.detail.keep_if;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Pred, typename Proj>
  concept removable_if_range =
    forward_range<Rng> and
    std::permutable<iterator_t<Rng>> and
    std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;
}

namespace genex::actions {
  struct remove_if_fn : pipe_closure {
    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::removable_if_range<Rng, Pred, Proj>
    constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> Rng {
      detail::keep_if(rng, [&]<typename T>(T &&e) -> bool {
        return not meta::invoke(pred, meta::invoke(proj, std::forward<T>(e)));
      });
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename Proj = meta::identity>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
      return meta::bind_back(remove_if_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr remove_if_fn remove_if{};
}
