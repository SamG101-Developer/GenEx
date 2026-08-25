module;
#include <genex/macros.hpp>

export module genex.views2.drop;
export import genex.pipe;
import genex.concepts;
import genex.subrange;
import genex.meta;
import genex.span;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept droppable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::weakly_incrementable<Int>;

  template <typename Rng, typename Int>
  concept droppable_range =
    input_range<Rng> and
    droppable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views {
  struct drop_fn {
    template <typename I, typename S, typename Int>
      requires detail::concepts::droppable_iters<I, S, Int>
      and std::contiguous_iterator<I> and std::sized_sentinel_for<S, I>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::span<iter_element_t<I>>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      const auto count = std::min(static_cast<iter_difference_t<I>>(n), last - first);
      return genex::span<iter_element_t<I>>(first + count, std::move(last));
    }

    template <typename I, typename S, typename Int>
      requires detail::concepts::droppable_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<I, S>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      return genex::subrange<I, S>(
        iterators::next(std::move(first), static_cast<iter_difference_t<I>>(n), last), std::move(last));
    }

    template <typename Rng, typename Int>
      requires detail::concepts::droppable_range<Rng, Int>
      and std::contiguous_iterator<iterator_t<Rng>> and std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::span<range_element_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      const auto count = std::min(static_cast<range_difference_t<Rng>>(n), last - first);
      return genex::span<range_element_t<Rng>>(first + count, std::move(last));
    }

    template <typename Rng, typename Int>
      requires detail::concepts::droppable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<iterator_t<Rng>, sentinel_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      return genex::subrange<iterator_t<Rng>, sentinel_t<Rng>>(
        iterators::next(std::move(first), static_cast<range_difference_t<Rng>>(n), last), std::move(last));
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
      SAFE_CTOR(drop_fn) and SAFE_MOVE(Int)) {
      return meta::bind_back(drop_fn{}, n);
    }
  };

  export inline constexpr drop_fn drop{};
}
