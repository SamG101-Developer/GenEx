module;
#include <genex/detail/macros.hpp>

export module genex.iterators.distance;
import genex.concepts;
import genex.iterators.iter_pair;
import std;

namespace genex::iterators::detail::concepts {
  template <typename I, typename S>
  concept distancable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    (std::same_as<I, S> or std::sized_sentinel_for<S, I>);

  template <typename Rng>
  concept distancable_range =
    input_range<Rng> and
    distancable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::iterators {
  struct distance_fn {
    template <typename I, typename S>
    requires detail::concepts::distancable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(
      I first, S last) const -> std::ptrdiff_t {
      if constexpr (std::sized_sentinel_for<S, I>) {
        return static_cast<std::ptrdiff_t>(last - first);
      }
      else {
        auto count = std::ptrdiff_t{0};
        for (; first != last; ++first) { ++count; }
        return count;
      }
    }

    template <typename Rng>
    requires detail::concepts::distancable_range<Rng>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const -> std::ptrdiff_t {
      auto [first, last] = iterators::iter_pair(rng);
      return (*this)(std::move(first), std::move(last));
    }
  };

  export inline constexpr distance_fn distance{};
}
