module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.binary_search;
import genex.algorithms.lower_bound;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename E, typename Comp, typename Proj>
  concept binary_searchable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    indirect_strict_weak_order<
      Comp, projected<I, Proj>, std::remove_cvref_t<E> const*>;

  template <typename Rng, typename E, typename Comp, typename Proj>
  concept binary_searchable_range =
    forward_range<Rng> and
    binary_searchable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Comp, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename E, typename Comp, typename Proj>
  requires concepts::binary_searchable_iters<I, S, E, Comp, Proj>
  GENEX_INLINE constexpr auto do_binary_search(
    I first, S last, E &&elem, Comp comp, Proj proj) -> bool {
    auto end = iterators::next(first, std::move(last));
    first = genex::lower_bound(first, end, elem, comp, proj);
    return first != end
      and not meta::invoke(comp, elem, meta::invoke(proj, *first));
  }
}

namespace genex {
  struct binary_search_fn {
    template <
      typename I, typename S, typename E, typename Comp = operations::lt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::binary_searchable_iters<
      I, S, E, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, E &&elem, Comp comp = {},
      Proj proj = {}) const -> bool {
      return algorithms::detail::impl::do_binary_search(
        std::move(first), std::move(last), std::forward<E>(elem),
        std::move(comp), std::move(proj));
    }

    template <
      typename Rng, typename E, typename Comp = operations::lt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::binary_searchable_range<
      Rng, E, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, E &&elem, Comp comp = {}, Proj proj = {}) const -> bool {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_binary_search(
        std::move(first), std::move(last), std::forward<E>(elem),
        std::move(comp), std::move(proj));
    }
  };

  export inline constexpr binary_search_fn binary_search{};
}
