module;
#include <genex/detail/macros.hpp>

export module genex.views.take;
export import genex.pipe;
import genex.concepts;
import genex.detail.counted_iterator;
import genex.iterators.iter_pair;
import genex.meta;
import genex.span;
import genex.subrange;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept takeable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::integral<Int>;

  template <typename Rng, typename Int>
  concept takeable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    takeable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views {
  struct take_fn : pipe_closure {
    template <typename I, typename S, typename Int>
    requires detail::concepts::takeable_iters<I, S, Int>
      and std::contiguous_iterator<I>
      and std::sized_sentinel_for<S, I>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::span<iter_element_t<I>>, I, I) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      const auto count = std::min(
        static_cast<iter_difference_t<I>>(n), last - first);
      return genex::span<iter_element_t<I>>(first, first + count);
    }

    template <typename I, typename S, typename Int>
    requires detail::concepts::takeable_iters<I, S, Int>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, const Int n) const noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      return genex::subrange(
        genex::detail::counted_iterator(
          std::move(first), static_cast<iter_difference_t<I>>(n)),
        genex::detail::counted_sentinel<S>{std::move(last)});
    }

    template <typename Rng, typename Int>
    requires detail::concepts::takeable_range<Rng, Int>
      and std::contiguous_iterator<iterator_t<Rng>>
      and std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, const Int n) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CTOR(genex::span<range_element_t<Rng>>, iterator_t<Rng>, iterator_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      const auto count = std::min(
        static_cast<range_difference_t<Rng>>(n), last - first);
      return genex::span<range_element_t<Rng>>(first, first + count);
    }

    template <typename Rng, typename Int>
    requires detail::concepts::takeable_range<Rng, Int>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, const Int n) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_MOVE(iterator_t<Rng>) and SAFE_MOVE(sentinel_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      return genex::subrange(
        genex::detail::counted_iterator(
          std::move(first), static_cast<range_difference_t<Rng>>(n)),
        genex::detail::counted_sentinel<sentinel_t<Rng>>{std::move(last)});
    }

    template <typename Int>
    requires std::integral<Int>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
      SAFE_CTOR(take_fn) and SAFE_MOVE(Int)) {
      return meta::bind_back(take_fn{}, n);
    }
  };

  export inline constexpr take_fn take{};
}
