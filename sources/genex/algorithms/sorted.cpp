module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.sorted;
import genex.algorithms.sort;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import genex.subrange;
import genex.to_container;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Comp, typename Proj>
  concept sorted_copyable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::copy_constructible<iter_value_t<I>> and
    sortable<typename std::vector<iter_value_t<I>>::iterator, Comp, Proj>;

  template <typename Rng, typename Comp, typename Proj>
  concept sorted_copyable_range =
    input_range<Rng> and
    sorted_copyable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Comp, typename Proj>
  requires concepts::sorted_copyable_iters<I, S, Comp, Proj>
  constexpr auto do_sorted(
    I first, S last, Comp comp, Proj proj) -> std::vector<iter_value_t<I>> {
    auto vec = genex::to<std::vector>()(
      genex::subrange(std::move(first), std::move(last)));
    genex::sort(vec, std::move(comp), std::move(proj));
    return vec;
  }
}

namespace genex {
  struct sorted_fn {
    template <
      typename I, typename S, typename Comp = operations::lt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::sorted_copyable_iters<
      I, S, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Comp comp = {},
      Proj proj = {}) const -> std::vector<iter_value_t<I>> {
      return algorithms::detail::impl::do_sorted(
        std::move(first), std::move(last),
        std::move(comp), std::move(proj));
    }

    template <
      typename Rng, typename Comp = operations::lt,
      typename Proj = meta::identity>
    requires algorithms::detail::concepts::sorted_copyable_range<
      Rng, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Comp comp = {},
      Proj proj = {}) const -> std::vector<range_value_t<Rng>> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_sorted(
        std::move(first), std::move(last),
        std::move(comp), std::move(proj));
    }
  };

  export inline constexpr sorted_fn sorted{};
}
