module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.contains_if;
import genex.algorithms.find_if;
import genex.concepts;
import genex.detail.algorithm_concepts;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept containable_if_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    algorithms::detail::concepts::findable_if_iters<I, S, Pred, Proj> and
    std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

  template <typename Rng, typename Pred, typename Proj>
  concept containable_if_range =
    input_range<Rng> and
    containable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::containable_if_iters<I, S, Pred, Proj>
  GENEX_INLINE constexpr auto do_contains_if(
    I first, S last, Pred pred, Proj proj) -> bool {
    for (; first != last; ++first) {
      if (meta::invoke(pred, meta::invoke(proj, *first))) { return true; }
    }
    return false;
  }
}

namespace genex {
  struct contains_if_fn {
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::containable_if_iters<I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const -> bool {
      return algorithms::detail::impl::do_contains_if(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::containable_if_range<Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> bool {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_contains_if(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }
  };

  export inline constexpr contains_if_fn contains_if{};
}
