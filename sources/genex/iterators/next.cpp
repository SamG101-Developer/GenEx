module;
#include <genex/detail/macros.hpp>

export module genex.iterators.next;
import genex.concepts;
import genex.iterators.advance;
import std;

namespace genex::iterators::detail::concepts {
  template <typename I, typename S = I>
  concept nextable_iters =
    std::input_or_output_iterator<I> and
    std::sentinel_for<S, I>;
}

namespace genex::iterators {
  struct next_fn {
    template <typename I>
      requires detail::concepts::nextable_iters<I> and std::copyable<I>
    GENEX_INLINE constexpr auto operator()(I it, const iter_difference_t<I> n = 1) const -> I {
      return iterators::advance(it, n);
    }

    template <typename I, typename S>
      requires detail::concepts::nextable_iters<I, S> and std::copyable<I>
    GENEX_INLINE constexpr auto operator()(I it, const iter_difference_t<I> n, const S bound) const -> I {
      if (n <= 0) { return it; }
      if constexpr (std::sized_sentinel_for<S, I> and std::random_access_iterator<I>) {
        // A branch and not a min: nearly every step misses the end, and a min would chain each one on the last.
        if (n < bound - it) [[likely]] { return it + n; }
        return it + (bound - it);
      }
      else {
        for (auto i = iter_difference_t<I>{0}; i < n and it != bound; ++i) { ++it; }
        return it;
      }
    }
  };

  export inline constexpr next_fn next{};
}
