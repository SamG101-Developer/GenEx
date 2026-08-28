module;
#include <genex/macros.hpp>

export module genex.actions.partition;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Pred, typename Proj>
  concept partitionable_range =
    forward_range<Rng> and
    std::permutable<iterator_t<Rng>> and
    std::same_as<iterator_t<Rng>, sentinel_t<Rng>> and
    std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;
}

namespace genex::actions {
  struct partition_fn {
    template <typename Rng, typename Pred, typename Proj = meta::identity>
      requires detail::concepts::partitionable_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const -> decltype(auto) {
      auto [first, last] = iterators::iter_pair(rng);
      auto partitioner = [&]<typename T>(T &&elem) {
        return meta::invoke(pred, meta::invoke(proj, std::forward<T>(elem)));
      };
      std::partition(std::move(first), std::move(last), std::move(partitioner));
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename Proj = meta::identity>
      requires (not range<Pred>)
    GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
      return meta::bind_back(partition_fn{}, std::move(pred), std::move(proj));
    }
  };

  struct stable_partition_fn {
    template <typename Rng, typename Pred, typename Proj = meta::identity>
      requires detail::concepts::partitionable_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const -> decltype(auto) {
      auto [first, last] = iterators::iter_pair(rng);
      auto partitioner = [&]<typename T>(T &&elem) {
        return meta::invoke(pred, meta::invoke(proj, std::forward<T>(elem)));
      };
      std::stable_partition(std::move(first), std::move(last), std::move(partitioner));
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename Proj = meta::identity>
      requires (not range<Pred>)
    GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
      return meta::bind_back(stable_partition_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr partition_fn partition{};
  export inline constexpr stable_partition_fn stable_partition{};
}