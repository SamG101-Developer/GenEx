module;
#include <genex/macros.hpp>

export module genex.algorithms.position;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept positionable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

  template <typename Rng, typename Pred, typename Proj>
  concept positionable_range =
    input_range<Rng> and
    positionable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::positionable_iters<I, S, Pred, Proj>
  GENEX_INLINE constexpr auto do_position(I first, S last, Pred &&pred, Proj &&proj,
    const std::size_t drop) -> std::optional<std::size_t> {
    for (auto i = drop; first != last; ++first, ++i) {
      if (meta::invoke(pred, meta::invoke(proj, *first))) { return i; }
    }
    return std::nullopt;
  }
}

namespace genex {
  struct position_fn {
    template <typename I, typename S, typename Pred, typename Proj = meta::identity>
      requires algorithms::detail::concepts::positionable_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {},
      const std::size_t drop = 0uz) const -> std::optional<std::size_t> {
      return algorithms::detail::impl::do_position(
        std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj), drop);
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
      requires algorithms::detail::concepts::positionable_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {},
      const std::size_t drop = 0uz) const -> std::optional<std::size_t> {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_position(
        std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj), drop);
    }
  };

  export inline constexpr position_fn position{};
}
