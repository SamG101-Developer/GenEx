module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.max_element;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Comp, typename Proj>
  concept maxable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_strict_weak_order<Comp, std::projected<I, Proj>>;

  template <typename Rng, typename Comp, typename Proj>
  concept maxable_range =
    input_range<Rng> and
    maxable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Comp, typename Proj>
  requires concepts::maxable_iters<I, S, Comp, Proj>
  constexpr auto do_max_element(
    I first, S last, Comp comp, Proj proj) -> iter_reference_t<I> {
    GENEX_ASSERT(first != last);
    for (auto next = first; ++next != last;) {
      if (meta::invoke(comp, meta::invoke(proj, *next), meta::invoke(proj, *first))) { first = next; }
    }
    return *first;
  }
}

namespace genex {
  struct max_element_fn {
    template <
      typename I, typename S, typename Comp = operations::gt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::maxable_iters<I, S, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(
      I first, S last, Comp comp = {},
      Proj proj = {}) const -> iter_reference_t<I> {
      return algorithms::detail::impl::do_max_element(
        std::move(first), std::move(last),
        std::move(comp), std::move(proj));
    }

    template <
      typename Rng, typename Comp = operations::gt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::maxable_range<Rng, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {},
      Proj proj = {}) const -> borrowed_reference_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_max_element(
        std::move(first), std::move(last),
        std::move(comp), std::move(proj));
    }
  };

  export inline constexpr max_element_fn max_element{};
}
