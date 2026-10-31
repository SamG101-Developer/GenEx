module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.reverse;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.iterators.iter_swap;
import genex.iterators.next;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S>
  concept reversible_iters =
    std::bidirectional_iterator<I> and
    std::sentinel_for<S, I> and
    permutable<I>;

  template <typename Rng>
  concept reversible_range =
    bidirectional_range<Rng> and
    reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::algorithms::detail::impl {
  template <typename I>
  constexpr auto do_reverse(I first, I last) -> void {
    if constexpr (std::random_access_iterator<I>) {
      // Knowing the count up front leaves a loop that can be vectorized.
      const auto swaps = (last - first) / 2;
      for (auto i = iter_difference_t<I>{0}; i < swaps; ++i) {
        --last;
        iterators::iter_swap(first, last);
        ++first;
      }
    }
    else {
      while (first != last and first != --last) {
        iterators::iter_swap(first, last);
        ++first;
      }
    }
  }
}

namespace genex {
  struct reverse_fn {
    template <typename I, typename S>
    requires algorithms::detail::concepts::reversible_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const -> I {
      auto end = iterators::next(first, std::move(last));
      algorithms::detail::impl::do_reverse(std::move(first), end);
      return end;
    }

    template <typename Rng>
    requires algorithms::detail::concepts::reversible_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> borrowed_iterator_t<Rng> {
      auto [first, last] = iterators::iter_pair(rng);
      return (*this)(std::move(first), std::move(last));
    }
  };

  export inline constexpr reverse_fn reverse{};
}
