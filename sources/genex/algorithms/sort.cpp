module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.sort;
import genex.concepts;
import genex.detail.projected_less;
import genex.detail.sort_impl;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Comp, typename Proj>
  concept sortable_iters =
    std::random_access_iterator<I> and
    std::sentinel_for<S, I> and
    sortable<I, Comp, Proj>;

  template <typename Rng, typename Comp, typename Proj>
  concept sortable_range =
    random_access_range<Rng> and
    sortable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}

namespace genex {
  struct sort_fn {
    template <typename I, typename S, typename Comp = operations::lt, typename Proj = meta::identity>
    requires algorithms::detail::concepts::sortable_iters<I, S, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(I first, S last, Comp comp = {}, Proj proj = {}) const -> I {
      auto end = iterators::next(first, std::move(last));
      auto less = genex::detail::projected_less<Comp, Proj>{comp, proj};
      algorithms::detail::sort_any(std::move(first), end, less);
      return end;
    }

    template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
    requires algorithms::detail::concepts::sortable_range<Rng, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Comp comp = {}, Proj proj = {}) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return (*this)(std::move(first), std::move(last), std::move(comp), std::move(proj));
    }
  };

  export inline constexpr sort_fn sort{};
}
