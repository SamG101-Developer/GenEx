module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.count_if;
import genex.concepts;
import genex.detail.predicate_iters;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept countable_if_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj>;

  template <typename Rng, typename Pred, typename Proj>
  concept countable_if_range =
    input_range<Rng> and
    countable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::countable_if_iters<I, S, Pred, Proj>
  constexpr auto do_count_if(
    I first, S last, Pred pred, Proj proj) -> std::size_t {
    auto count = 0uz;
    for (; first != last; ++first) {
      if (meta::invoke(pred, meta::invoke(proj, *first))) { ++count; }
    }
    return count;
  }
}

namespace genex {
  struct count_if_fn {
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::countable_if_iters<I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const -> std::size_t {
      return algorithms::detail::impl::do_count_if(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::countable_if_range<Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> std::size_t {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_count_if(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }
  };

  export inline constexpr count_if_fn count_if{};
}
