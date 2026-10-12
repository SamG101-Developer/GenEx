module;
#include <genex/macros.hpp>

export module genex.algorithms.position_last;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept positionable_last_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

  template <typename Rng, typename Pred, typename Proj>
  concept positionable_last_range =
    forward_range<Rng> and
    positionable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::positionable_last_iters<I, S, Pred, Proj>
    and std::bidirectional_iterator<I>

  GENEX_INLINE constexpr auto do_position_last(I first, S last, Pred &&pred, Proj &&proj)
    -> std::optional<std::size_t> {
    auto pos = static_cast<std::size_t>(iterators::distance(first, last));
    while (first != last) {
      --last;
      --pos;
      if (meta::invoke(pred, meta::invoke(proj, *last))) { return pos; }
    }
    return std::nullopt;
  }

  template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::positionable_last_iters<I, S, Pred, Proj>
  GENEX_INLINE constexpr auto do_position_last(I first, S last, Pred &&pred, Proj &&proj)
    -> std::optional<std::size_t> {
    auto pos = std::optional<std::size_t>();
    for (auto i = 0uz; first != last; ++first, ++i) {
      if (meta::invoke(pred, meta::invoke(proj, *first))) { pos = i; }
    }
    return pos;
  }
}

namespace genex {
  struct position_last_fn {
    template <typename I, typename S, typename Pred, typename Proj = meta::identity>
      requires algorithms::detail::concepts::positionable_last_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const
      -> std::optional<std::size_t> {
      return algorithms::detail::impl::do_position_last(
        std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
      requires algorithms::detail::concepts::positionable_last_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const
      -> std::optional<std::size_t> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_position_last(
        std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
    }
  };

  export inline constexpr position_last_fn position_last{};
}
