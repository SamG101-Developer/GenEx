module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.shuffle;
import genex.concepts;
import genex.detail.bounded_random;
import genex.iterators.iter_pair;
import genex.iterators.iter_swap;
import genex.iterators.next;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S, typename G>
  concept shufflable_iters =
    std::random_access_iterator<I> and
    std::sentinel_for<S, I> and
    permutable<I> and
    std::uniform_random_bit_generator<std::remove_reference_t<G>>;

  template <typename Rng, typename G>
  concept shufflable_range =
    random_access_range<Rng> and
    shufflable_iters<iterator_t<Rng>, sentinel_t<Rng>, G>;
}

namespace genex {
  struct shuffle_fn {
    template <typename I, typename S, typename G>
    requires algorithms::detail::concepts::shufflable_iters<I, S, G>
    constexpr auto operator()(I first, S last, G &&generator) const -> I {
      using diff_t = iter_difference_t<I>;
      auto end = iterators::next(first, std::move(last));
      auto random = detail::bounded_random<std::remove_reference_t<G>>(generator);

      for (auto i = end - first; i > 1; --i) {
        const auto j = random.below(static_cast<std::uint64_t>(i));
        iterators::iter_swap(first + (i - 1), first + static_cast<diff_t>(j));
      }
      return end;
    }

    template <typename Rng, typename G>
    requires algorithms::detail::concepts::shufflable_range<Rng, G>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, G &&generator) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return (*this)(std::move(first), std::move(last), std::forward<G>(generator));
    }
  };

  export inline constexpr shuffle_fn shuffle{};
}
