module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.find_if_not;
import genex.concepts;
import genex.detail.predicate_iters;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept findable_if_not_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj>;

  template <typename Rng, typename Pred, typename Proj>
  concept findable_if_not_range =
    input_range<Rng> and
    findable_if_not_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::findable_if_not_iters<I, S, Pred, Proj>
  constexpr auto do_find_if_not(
    I first, S last, Pred pred, Proj proj) -> I {
    for (; first != last; ++first) {
      if (not meta::invoke(pred, meta::invoke(proj, *first))) { break; }
    }
    return first;
  }
}

namespace genex {
  struct find_if_not_fn {
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_if_not_iters<
      I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const -> I {
      return algorithms::detail::impl::do_find_if_not(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_if_not_range<
      Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_find_if_not(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }
  };

  export inline constexpr find_if_not_fn find_if_not{};
}
