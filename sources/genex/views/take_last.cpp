module;
#include <genex/macros.hpp>

export module genex.views2.take_last;
export import genex.pipe;
import genex.concepts;
import genex.subrange;
import genex.meta;
import genex.span;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept takeable_last_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::weakly_incrementable<Int>;

  template <typename Rng, typename Int>
  concept takeable_last_range =
    input_range<Rng> and
    takeable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views {
  struct take_last_fn {
    template <typename I, typename S, typename Int>
      requires detail::concepts::takeable_last_iters<I, S, Int>
      and std::contiguous_iterator<I> and std::same_as<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::span<iter_element_t<I>>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      const auto count = std::min(static_cast<iter_difference_t<I>>(n), last - first);
      return genex::span<iter_element_t<I>>(last - count, std::move(last));
    }

    template <typename I, typename S, typename Int>
      requires detail::concepts::takeable_last_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<I, S>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      auto probe = iterators::next(first, static_cast<iter_difference_t<I>>(n), last);
      auto start = first;
      while (probe != last) {
        ++probe;
        ++start;
      }
      return genex::subrange<I, S>(std::move(start), std::move(last));
    }

    template <typename Rng, typename Int>
      requires detail::concepts::takeable_last_range<Rng, Int>
      and std::contiguous_iterator<iterator_t<Rng>> and std::same_as<iterator_t<Rng>, sentinel_t<Rng>>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::span<range_element_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      const auto count = std::min(static_cast<range_difference_t<Rng>>(n), last - first);
      return genex::span<range_element_t<Rng>>(last - count, std::move(last));
    }

    template <typename Rng, typename Int>
      requires detail::concepts::takeable_last_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<iterator_t<Rng>, sentinel_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      auto probe = iterators::next(first, static_cast<range_difference_t<Rng>>(n), last);
      auto start = first;
      while (probe != last) {
        ++probe;
        ++start;
      }
      return genex::subrange<iterator_t<Rng>, sentinel_t<Rng>>(std::move(start), std::move(last));
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
      SAFE_CTOR(take_last_fn) and SAFE_MOVE(Int)) {
      return meta::bind_back(take_last_fn{}, n);
    }
  };

  export inline constexpr take_last_fn take_last{};
}
