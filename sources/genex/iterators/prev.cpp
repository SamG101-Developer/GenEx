module;
#include <genex/macros.hpp>

export module genex.iterators.prev;
import genex.concepts;
import std;

namespace genex::iterators::detail::concepts {
  template <typename I>
  concept prevable_iters =
    std::bidirectional_iterator<I>;
}

namespace genex::iterators {
  struct prev_fn {
    template <typename I>
      requires detail::concepts::prevable_iters<I> and std::copyable<I>
    GENEX_INLINE constexpr auto operator()(I it, const iter_difference_t<I> n = 1) const -> I {
      return std::prev(std::move(it), n);
    }

    template <typename I>
      requires detail::concepts::prevable_iters<I> and std::copyable<I>
    GENEX_INLINE constexpr auto operator()(I it, const iter_difference_t<I> n, const I bound) const -> I {
      if (n <= 0) { return it; }
      if constexpr (std::random_access_iterator<I>) {
        const auto remaining = it - bound;
        return it - (n < remaining ? n : remaining);
      }
      else {
        for (auto i = iter_difference_t<I>{0}; i < n and it != bound; ++i) { --it; }
        return it;
      }
    }
  };

  export inline constexpr prev_fn prev{};
}
