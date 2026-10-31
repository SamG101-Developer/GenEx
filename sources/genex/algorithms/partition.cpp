module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.partition;
import genex.concepts;
import genex.detail.partition_impl;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept partitionable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    permutable<I> and
    indirect_unary_predicate<Pred, projected<I, Proj>>;

  template <typename Rng, typename Pred, typename Proj>
  concept partitionable_range =
    forward_range<Rng> and
    partitionable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex {
  struct partition_fn {
    template <typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::partitionable_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const -> I {
      auto end = iterators::next(first, std::move(last));
      auto test = [&]<typename T>(T &&elem) -> bool {
        return meta::invoke(pred, meta::invoke(proj, std::forward<T>(elem)));
      };
      return algorithms::detail::partition_any(std::move(first), std::move(end), test);
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires algorithms::detail::concepts::partitionable_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return (*this)(std::move(first), std::move(last), std::move(pred), std::move(proj));
    }
  };

  export inline constexpr partition_fn partition{};
}
