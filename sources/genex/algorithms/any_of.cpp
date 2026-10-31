module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.any_of;
import genex.concepts;
import genex.detail.algorithm_concepts;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::quantifiable_iters<I, S, Pred, Proj>
  GENEX_INLINE constexpr auto do_any_of(
    I first, S last, Pred pred, Proj proj) -> bool {
    for (; first != last; ++first) {
      if (meta::invoke(pred, meta::invoke(proj, *first))) { return true; }
    }
    return false;
  }
}

namespace genex {
  struct any_of_fn {
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::quantifiable_iters<I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const -> bool {
      return algorithms::detail::impl::do_any_of(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::quantifiable_range<Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> bool {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_any_of(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }
  };

  export inline constexpr any_of_fn any_of{};
}
