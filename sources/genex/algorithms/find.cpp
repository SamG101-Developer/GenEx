module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.find;
import genex.concepts;
import genex.detail.algorithm_concepts;
import genex.detail.libc_scan;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename Rng, typename E, typename Proj>
  concept findable_range =
    input_range<Rng> and
    findable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;

  template <typename I, typename S, typename E, typename Proj>
  concept libc_findable_iters =
    findable_iters<I, S, E, Proj> and
    genex::detail::libc_find_usable<I, S, E, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename E, typename Proj>
  GENEX_INLINE constexpr auto find_by_element(I first, S last, E &elem, Proj &proj) -> I {
    for (; first != last; ++first) {
      if (meta::invoke(proj, *first) == elem) { break; }
    }
    return first;
  }

  template <typename I, typename S, typename E, typename Proj>
  requires concepts::findable_iters<I, S, E, Proj>
  constexpr auto do_find(I first, S last, E &&elem, Proj proj) -> I {
    return find_by_element(std::move(first), std::move(last), elem, proj);
  }

  // libc can't be called at compile time, which needs the elements comparing.
  template <typename I, typename S, typename E, typename Proj>
  requires concepts::libc_findable_iters<I, S, E, Proj>
  constexpr auto do_find(I first, S last, E &&elem, Proj proj) -> I {
    if consteval {
      return find_by_element(std::move(first), std::move(last), elem, proj);
    }
    else {
      return genex::detail::libc_find_iters(std::move(first), last, elem);
    }
  }
}

namespace genex {
  struct find_fn {
    template <typename I, typename S, typename E, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_iters<I, S, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, E &&elem, Proj proj = {}) const -> I {
      return algorithms::detail::impl::do_find(
        std::move(first), std::move(last),
        std::forward<E>(elem), std::move(proj));
    }

    template <typename Rng, typename E, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_range<Rng, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, E &&elem, Proj proj = {}) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_find(
        std::move(first), std::move(last),
        std::forward<E>(elem), std::move(proj));
    }
  };

  export inline constexpr find_fn find{};
}
