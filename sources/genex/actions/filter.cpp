module;
#include <genex/detail/macros.hpp>

export module genex.actions.filter;
export import genex.pipe;
import genex.concepts;
import genex.detail.keep_if;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Pred, typename Proj>
  concept filterable_range =
    forward_range<Rng> and
    has_member_erase<Rng> and
    indirect_unary_predicate<Pred, projected<iterator_t<Rng>, Proj>>;
}

namespace genex::actions {
  struct filter_fn : pipe_closure {
    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::filterable_range<Rng, Pred, Proj>
    constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> Rng {
      detail::keep_if(rng, [&]<typename T>(T &&elem) -> bool {
        return meta::invoke(pred, meta::invoke(proj, std::forward<T>(elem)));
      });
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename Proj = meta::identity>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Pred pred, Proj proj = {}) const noexcept(
      SAFE_CTOR(filter_fn) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      return meta::bind_back(filter_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr filter_fn filter{};
}