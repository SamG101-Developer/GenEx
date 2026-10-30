module;
#include <genex/macros.hpp>

export module genex.views2.take;
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
  concept takeable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::weakly_incrementable<Int>;

  template <typename Rng, typename Int>
  concept takeable_range =
    input_range<Rng> and
    takeable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views::detail::impl {
  template <typename S>
  struct take_sentinel {
    GENEX_NO_UNIQUE_ADDRESS S st;

    template <typename I>
    GENEX_INLINE friend constexpr auto operator==(const std::counted_iterator<I> &it,
      const take_sentinel &self) -> bool {
      return it.count() == 0 or it.base() == self.st;
    }

    template <typename I>
      requires std::sized_sentinel_for<S, I>
    GENEX_INLINE friend constexpr auto operator-(const take_sentinel &self,
      const std::counted_iterator<I> &it) -> std::iter_difference_t<I> {
      return std::min(it.count(), self.st - it.base());
    }

    template <typename I>
      requires std::sized_sentinel_for<S, I>
    GENEX_INLINE friend constexpr auto operator-(const std::counted_iterator<I> &it,
      const take_sentinel &self) -> std::iter_difference_t<I> {
      return -(self - it);
    }
  };
}

namespace genex::views {
  struct take_fn {
    template <typename I, typename S, typename Int>
      requires detail::concepts::takeable_iters<I, S, Int>
      and std::contiguous_iterator<I> and std::sized_sentinel_for<S, I>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::span<iter_element_t<I>>, I, I) and SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      const auto count = std::min(static_cast<iter_difference_t<I>>(n), last - first);
      return genex::span<iter_element_t<I>>(first, first + count);
    }

    template <typename I, typename S, typename Int>
      requires detail::concepts::takeable_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      return genex::subrange(
        std::counted_iterator(std::move(first), static_cast<iter_difference_t<I>>(n)),
        detail::impl::take_sentinel<S>{std::move(last)});
    }

    template <typename Rng, typename Int>
      requires detail::concepts::takeable_range<Rng, Int>
      and std::contiguous_iterator<iterator_t<Rng>> and std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::span<range_element_t<Rng>>, iterator_t<Rng>, iterator_t<Rng>) and SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      const auto count = std::min(static_cast<range_difference_t<Rng>>(n), last - first);
      return genex::span<range_element_t<Rng>>(first, first + count);
    }

    template <typename Rng, typename Int>
      requires detail::concepts::takeable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_MOVE(iterator_t<Rng>) and SAFE_MOVE(sentinel_t<Rng>) and SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      return genex::subrange(
        std::counted_iterator(std::move(first), static_cast<range_difference_t<Rng>>(n)),
        detail::impl::take_sentinel<sentinel_t<Rng>>{std::move(last)});
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
      SAFE_CTOR(take_fn) and SAFE_MOVE(Int)) {
      return meta::bind_back(take_fn{}, n);
    }
  };

  export inline constexpr take_fn take{};
}
