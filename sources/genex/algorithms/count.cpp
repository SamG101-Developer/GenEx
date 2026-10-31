module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.count;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename E, typename Proj>
  concept countable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    indirect_equivalence_relation<
      operations::eq, projected<I, Proj>, std::remove_cvref_t<E> const*>;

  template <typename Rng, typename E, typename Proj>
  concept countable_range =
    input_range<Rng> and
    countable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename E, typename Proj>
  requires concepts::countable_iters<I, S, E, Proj>
  constexpr auto do_count(
    I first, S last, E &&elem, Proj proj) -> std::size_t {
    auto count = iter_difference_t<I>{0};
    for (; first != last; ++first) {
      if (meta::invoke(proj, *first) == elem) { ++count; }
    }
    return static_cast<std::size_t>(count);
  }
}

namespace genex {
  struct count_fn {
    template <
      typename I, typename S, typename E, typename Proj = meta::identity>
    requires algorithms::detail::concepts::countable_iters<I, S, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, E &&elem, Proj proj = {}) const -> std::size_t {
      return algorithms::detail::impl::do_count(
        std::move(first), std::move(last),
        std::forward<E>(elem), std::move(proj));
    }

    template <typename Rng, typename E, typename Proj = meta::identity>
    requires algorithms::detail::concepts::countable_range<Rng, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, E &&elem, Proj proj = {}) const -> std::size_t {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_count(
        std::move(first), std::move(last),
        std::forward<E>(elem), std::move(proj));
    }
  };

  export inline constexpr count_fn count{};
}
