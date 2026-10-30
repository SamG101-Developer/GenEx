module;
#include <genex/macros.hpp>

export module genex.views2.slice;
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
  concept sliceable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::weakly_incrementable<Int>;

  template <typename Rng, typename Int>
  concept sliceable_range =
    input_range<Rng> and
    sliceable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views::detail::impl {
  template <typename S>
  struct slice_sentinel {
    GENEX_NO_UNIQUE_ADDRESS S st;

    template <typename I>
    GENEX_INLINE friend constexpr auto operator==(const std::counted_iterator<I> &it,
      const slice_sentinel &self) -> bool {
      return it.count() == 0 or it.base() == self.st;
    }

    template <typename I>
      requires std::sized_sentinel_for<S, I>
    GENEX_INLINE friend constexpr auto operator-(const slice_sentinel &self,
      const std::counted_iterator<I> &it) -> std::iter_difference_t<I> {
      return std::min(it.count(), self.st - it.base());
    }

    template <typename I>
      requires std::sized_sentinel_for<S, I>
    GENEX_INLINE friend constexpr auto operator-(const std::counted_iterator<I> &it,
      const slice_sentinel &self) -> std::iter_difference_t<I> {
      return -(self - it);
    }
  };
}

namespace genex::views {
  struct slice_fn {
    template <typename I, typename S, typename Int>
      requires detail::concepts::sliceable_iters<I, S, Int>
      and std::contiguous_iterator<I> and std::sized_sentinel_for<S, I>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n, const Int m) const noexcept(
      SAFE_CTOR(genex::span<iter_element_t<I>>, I, I) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      const auto size = last - first;
      const auto lo = std::min(static_cast<iter_difference_t<I>>(n), size);
      const auto hi = std::clamp(static_cast<iter_difference_t<I>>(m), lo, size);
      return genex::span<iter_element_t<I>>(first + lo, first + hi);
    }

    template <typename I, typename S, typename Int>
      requires detail::concepts::sliceable_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n, const Int m) const noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      const auto lo = static_cast<iter_difference_t<I>>(n);
      const auto hi = static_cast<iter_difference_t<I>>(m);
      return genex::subrange(
        std::counted_iterator(iterators::next(std::move(first), lo, last), std::max(hi - lo, iter_difference_t<I>{0})),
        detail::impl::slice_sentinel<S>{std::move(last)});
    }

    template <typename Rng, typename Int>
      requires detail::concepts::sliceable_range<Rng, Int>
      and std::contiguous_iterator<iterator_t<Rng>> and std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n, const Int m) const noexcept(
      SAFE_CTOR(genex::span<range_element_t<Rng>>, iterator_t<Rng>, iterator_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      const auto size = last - first;
      const auto lo = std::min(static_cast<range_difference_t<Rng>>(n), size);
      const auto hi = std::clamp(static_cast<range_difference_t<Rng>>(m), lo, size);
      return genex::span<range_element_t<Rng>>(first + lo, first + hi);
    }

    template <typename Rng, typename Int>
      requires detail::concepts::sliceable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n, const Int m) const noexcept(
      SAFE_MOVE(iterator_t<Rng>) and SAFE_MOVE(sentinel_t<Rng>) and SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      const auto lo = static_cast<range_difference_t<Rng>>(n);
      const auto hi = static_cast<range_difference_t<Rng>>(m);
      return genex::subrange(
        std::counted_iterator(
          iterators::next(std::move(first), lo, last), std::max(hi - lo, range_difference_t<Rng>{0})),
        detail::impl::slice_sentinel<sentinel_t<Rng>>{std::move(last)});
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n, const Int m) const noexcept(
      SAFE_CTOR(slice_fn) and SAFE_MOVE(Int)) {
      return meta::bind_back(slice_fn{}, n, m);
    }
  };

  export inline constexpr slice_fn slice{};
}
