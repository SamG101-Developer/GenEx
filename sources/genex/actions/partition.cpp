module;
#include <genex/detail/macros.hpp>

export module genex.actions.partition;
export import genex.pipe;
import genex.algorithms.partition;
import genex.algorithms.stable_partition;
import genex.concepts;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Pred, typename Proj>
  concept partitionable_range =
    forward_range<Rng> and
    permutable<iterator_t<Rng>> and
    std::same_as<iterator_t<Rng>, sentinel_t<Rng>> and
    indirect_unary_predicate<Pred, projected<iterator_t<Rng>, Proj>>;
}

namespace genex::actions {
  struct partition_fn : pipe_closure {
    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::partitionable_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> Rng {
      genex::partition(rng, std::move(pred), std::move(proj));
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename Proj = meta::identity>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Pred pred, Proj proj = {}) const {
      return meta::bind_back(
        partition_fn{}, std::move(pred), std::move(proj));
    }
  };

  struct stable_partition_fn : pipe_closure {
    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::partitionable_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const -> Rng {
      genex::stable_partition(rng, std::move(pred), std::move(proj));
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename Proj = meta::identity>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Pred pred, Proj proj = {}) const {
      return meta::bind_back(
        stable_partition_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr partition_fn partition{};
  export inline constexpr stable_partition_fn stable_partition{};
}
