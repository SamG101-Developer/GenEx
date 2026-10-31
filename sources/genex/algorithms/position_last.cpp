module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.position_last;
import genex.algorithms.find_if;
import genex.concepts;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept positionable_last_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    indirect_unary_predicate<Pred, projected<I, Proj>>;

  template <typename Rng, typename Pred, typename Proj>
  concept positionable_last_range =
    forward_range<Rng> and
    positionable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::positionable_last_iters<I, S, Pred, Proj>
    and std::bidirectional_iterator<I> and std::convertible_to<S, I>
  constexpr auto do_position_last(I first, S last, Pred pred, Proj proj)
    -> std::optional<std::size_t> {
    const auto rend = std::reverse_iterator<I>(first);
    auto it = genex::find_if(
      std::reverse_iterator<I>(static_cast<I>(std::move(last))), rend,
      std::move(pred), std::move(proj));
    if (it == rend) { return std::nullopt; }
    return static_cast<std::size_t>(iterators::distance(first, it.base())) - 1;
  }

  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::positionable_last_iters<I, S, Pred, Proj>
  constexpr auto do_position_last(I first, S last, Pred pred, Proj proj)
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
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::positionable_last_iters<
      I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const
      -> std::optional<std::size_t> {
      return algorithms::detail::impl::do_position_last(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::positionable_last_range<
      Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const
      -> std::optional<std::size_t> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_position_last(
        std::move(first), std::move(last),
        std::move(pred), std::move(proj));
    }
  };

  export inline constexpr position_last_fn position_last{};
}
