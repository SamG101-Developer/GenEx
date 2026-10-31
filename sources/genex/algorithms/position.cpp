module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.position;
import genex.concepts;
import genex.detail.predicate_iters;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept positionable_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj>;

  template <typename Rng, typename Pred, typename Proj>
  concept positionable_range =
    input_range<Rng> and
    positionable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::positionable_iters<I, S, Pred, Proj>
  constexpr auto do_position(I first, S last, Pred pred, Proj proj,
    const std::size_t drop) -> std::optional<std::size_t> {
    for (auto i = drop; first != last; ++first, ++i) {
      if (meta::invoke(pred, meta::invoke(proj, *first))) { return i; }
    }
    return std::nullopt;
  }
}

namespace genex {
  struct position_fn {
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::positionable_iters<I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {},
      const std::size_t drop = 0uz) const -> std::optional<std::size_t> {
      return algorithms::detail::impl::do_position(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj), drop);
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
      requires algorithms::detail::concepts::positionable_range<Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {},
      const std::size_t drop = 0uz) const -> std::optional<std::size_t> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_position(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj), drop);
    }
  };

  export inline constexpr position_fn position{};
}
