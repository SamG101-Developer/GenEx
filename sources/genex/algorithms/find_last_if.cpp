module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.find_last_if;
import genex.algorithms.find_if;
import genex.concepts;
import genex.detail.predicate_iters;
import genex.iterators.iter_pair;
import genex.iterators.prev;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept findable_last_if_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj>;

  template <typename Rng, typename Pred, typename Proj>
  concept findable_last_if_range =
    input_range<Rng> and
    findable_last_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::findable_last_if_iters<I, S, Pred, Proj>
    and std::bidirectional_iterator<I> and std::convertible_to<S, I>
  constexpr auto do_find_last_if(
    I first, S last, Pred pred, Proj proj) -> I {
    auto end = static_cast<I>(std::move(last));
    const auto rend = std::reverse_iterator<I>(std::move(first));
    auto it = genex::find_if(
      std::reverse_iterator<I>(end), rend,
      std::move(pred), std::move(proj));
    return it == rend ? end : iterators::prev(it.base());
  }

  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::findable_last_if_iters<I, S, Pred, Proj>
  constexpr auto do_find_last_if(
    I first, S last, Pred pred, Proj proj) -> I {
    auto result = first;
    auto found = false;
    for (; first != last; ++first) {
      if (meta::invoke(pred, meta::invoke(proj, *first))) {
        result = first;
        found = true;
      }
    }
    return found ? result : first;
  }
}

namespace genex {
  struct find_last_if_fn {
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_last_if_iters<
      I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const -> I {
      return algorithms::detail::impl::do_find_last_if(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }

    template <
      typename Rng, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::findable_last_if_range<
      Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_find_last_if(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }
  };

  export inline constexpr find_last_if_fn find_last_if{};
}
