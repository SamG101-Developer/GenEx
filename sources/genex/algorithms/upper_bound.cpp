module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.upper_bound;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename E, typename Comp, typename Proj>
  concept upper_boundable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    indirect_strict_weak_order<
      Comp, projected<I, Proj>, std::remove_cvref_t<E> const*>;

  template <typename Rng, typename E, typename Comp, typename Proj>
  concept upper_boundable_range =
    forward_range<Rng> and
    upper_boundable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Comp, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename E, typename Comp, typename Proj>
  GENEX_INLINE constexpr auto upper_bound_goes_after(
    const I &it, const E &elem, Comp &comp, Proj &proj) -> bool {
    return not meta::invoke(comp, elem, meta::invoke(proj, *it));
  }

  template <typename I, typename E, typename Comp, typename Proj>
  constexpr auto do_upper_bound(I first, I last, const E &elem, Comp &comp, Proj &proj) -> I {
    using diff_t = iter_difference_t<I>;
    if constexpr (std::random_access_iterator<I>) {
      auto count = last - first;
      if (count == 0) { return first; }

      // The answer steps along by arithmetic, not a branch. A binary search's
      // branches go each way at random, so they'd be mispredicted half the time.
      while (count > 1) {
        const auto half = count / 2;
        const auto go_right = upper_bound_goes_after(first + (half - 1), elem, comp, proj);
        first += half & -static_cast<diff_t>(go_right);
        count -= half;
      }
      return first + static_cast<diff_t>(upper_bound_goes_after(first, elem, comp, proj));
    }
    else {
      auto count = diff_t{0};
      for (auto it = first; it != last; ++it) { ++count; }

      while (count > 0) {
        const auto half = count / 2;
        auto mid = iterators::next(first, half);
        if (upper_bound_goes_after(mid, elem, comp, proj)) {
          first = std::move(++mid);
          count -= half + 1;
        }
        else {
          count = half;
        }
      }
      return first;
    }
  }
}

namespace genex {
  struct upper_bound_fn {
    template <
      typename I, typename S, typename E, typename Comp = operations::lt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::upper_boundable_iters<I, S, E, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, const E &elem, Comp comp = {}, Proj proj = {}) const -> I {
      auto end = iterators::next(first, std::move(last));
      return algorithms::detail::impl::do_upper_bound(
        std::move(first), std::move(end), elem, comp, proj);
    }

    template <
      typename Rng, typename E, typename Comp = operations::lt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::upper_boundable_range<Rng, E, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, const E &elem, Comp comp = {}, Proj proj = {}) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return (*this)(std::move(first), std::move(last), elem, std::move(comp), std::move(proj));
    }
  };

  export inline constexpr upper_bound_fn upper_bound{};
}
