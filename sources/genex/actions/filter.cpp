module;
#include <genex/detail/macros.hpp>

export module genex.actions.filter;
export import genex.pipe;
import genex.actions.erase;
import genex.concepts;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Pred, typename Proj>
  concept filterable_range =
    forward_range<Rng> and
    has_member_erase<Rng> and
    std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;
}

namespace genex::actions {
  struct filter_fn {
    template <typename Rng, typename Pred, typename Proj = meta::identity>
      requires detail::concepts::filterable_range<Rng, Pred, Proj>
    constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const -> Rng {
      detail::keep_if(rng, [&]<typename T>(T &&elem) -> bool {
        return meta::invoke(pred, meta::invoke(proj, std::forward<T>(elem)));
      });
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename Proj = meta::identity>
      requires (not range<Pred>)
    GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
      return meta::bind_back(filter_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr filter_fn filter{};
}