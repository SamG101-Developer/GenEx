module;
#include <genex/detail/macros.hpp>

export module genex.views.map;
export import genex.pipe;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.tuple;
import genex.views.transform;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept mappable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    pair_like<iter_reference_t<I>>;

  template <typename Rng>
  concept mappable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    mappable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views {
  template <std::size_t N>
  struct map_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::mappable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last) const noexcept(
      SAFE_CALL(decltype(transform), I, S, meta::identity)) {
      return transform(std::move(first), std::move(last), genex::get<N>);
    }

    template <typename Rng>
    requires detail::concepts::mappable_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CALL(decltype(transform), iterator_t<Rng>, sentinel_t<Rng>, meta::identity)) {
      auto [first, last] = iterators::iter_pair(rng);
      return transform(std::move(first), std::move(last), genex::get<N>);
    }
  };

  export inline constexpr map_fn<0> keys{};
  export inline constexpr map_fn<1> vals{};
}
