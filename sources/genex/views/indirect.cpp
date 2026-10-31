module;
#include <genex/detail/macros.hpp>

export module genex.views.indirect;
export import genex.pipe;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.views.transform;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept indirectable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirectly_readable<I> and
    requires(I it) { { **it }; };

  template <typename Rng>
  concept indirectable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    indirectable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views {
  struct indirect_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::indirectable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_CALL(decltype(transform), I, S, meta::identity) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return transform(
        std::move(first), std::move(last),
        [](auto &&ptr) -> decltype(auto) { return *ptr; });
    }

    template <typename Rng>
    requires detail::concepts::indirectable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CALL(decltype(transform), iterator_t<Rng>, sentinel_t<Rng>, meta::identity)) {
      auto [first, last] = iterators::iter_pair(rng);
      return transform(
        std::move(first), std::move(last),
        [](auto &&ptr) -> decltype(auto) { return *ptr; });
    }
  };

  export inline constexpr indirect_fn indirect{};
}
