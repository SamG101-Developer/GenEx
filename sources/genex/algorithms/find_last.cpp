module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.find_last;
import genex.algorithms.find_if;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.iterators.prev;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename E, typename Proj>
  concept findable_last_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    indirect_equivalence_relation<
      operations::eq, projected<I, Proj>, std::remove_cvref_t<E> const*>;

  template <typename Rng, typename E, typename Proj>
  concept findable_last_range =
    input_range<Rng> and
    findable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename E, typename Proj>
  requires concepts::findable_last_iters<I, S, E, Proj>
    and std::bidirectional_iterator<I> and std::convertible_to<S, I>
  constexpr auto do_find_last(I first, S last, E &&elem, Proj proj) -> I {
    auto end = static_cast<I>(std::move(last));
    const auto rend = std::reverse_iterator<I>(std::move(first));
    auto it = genex::find_if(
      std::reverse_iterator<I>(end), rend,
      [&elem](const auto &value) { return value == elem; }, std::move(proj));
    return it == rend ? end : iterators::prev(it.base());
  }

  template <typename I, typename S, typename E, typename Proj>
  requires concepts::findable_last_iters<I, S, E, Proj>
  constexpr auto do_find_last(
    I first, S last, E &&elem, Proj proj) -> I {
    auto result = first;
    auto found = false;
    for (; first != last; ++first) {
      if (meta::invoke(proj, *first) == elem) {
        result = first;
        found = true;
      }
    }
    return found ? result : first;
  }
}

namespace genex {
  struct find_last_fn {
    template <typename I, typename S, typename E, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_last_iters<I, S, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, E &&elem, Proj proj = {}) const -> I {
      return algorithms::detail::impl::do_find_last(
        std::move(first), std::move(last),
        std::forward<E>(elem), std::move(proj));
    }

    template <typename Rng, typename E, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_last_range<Rng, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, E &&elem, Proj proj = {}) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_find_last(
        std::move(first), std::move(last),
        std::forward<E>(elem), std::move(proj));
    }
  };

  export inline constexpr find_last_fn find_last{};
}
