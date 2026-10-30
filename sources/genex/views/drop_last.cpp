module;
#include <genex/macros.hpp>

export module genex.views2.drop_last;
export import genex.pipe;
import genex.concepts;
import genex.subrange;
import genex.meta;
import genex.span;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.iterators.prev;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept droppable_last_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::weakly_incrementable<Int>;

  template <typename Rng, typename Int>
  concept droppable_last_range =
    input_range<Rng> and
    droppable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views {
  struct drop_last_fn {
    template <typename I, typename S, typename Int>
      requires detail::concepts::droppable_last_iters<I, S, Int>
      and std::contiguous_iterator<I> and std::same_as<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::span<iter_element_t<I>>, I, I) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      const auto size = last - first;
      const auto count = std::min(static_cast<iter_difference_t<I>>(n), size);
      return genex::span<iter_element_t<I>>(first, first + (size - count));
    }

    template <typename I, typename S, typename Int>
      requires detail::concepts::droppable_last_iters<I, S, Int>
      and std::bidirectional_iterator<I> and std::same_as<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<I, S>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      auto stop = iterators::prev(std::move(last), static_cast<iter_difference_t<I>>(n), first);
      return genex::subrange<I, S>(std::move(first), std::move(stop));
    }

    template <typename I, typename S, typename Int>
      requires detail::concepts::droppable_last_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<I, I>, I, I) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      auto probe = iterators::next(first, static_cast<iter_difference_t<I>>(n), last);
      auto stop = first;
      while (probe != last) {
        ++probe;
        ++stop;
      }
      return genex::subrange<I, I>(std::move(first), std::move(stop));
    }

    template <typename Rng, typename Int>
      requires detail::concepts::droppable_last_range<Rng, Int>
      and std::contiguous_iterator<iterator_t<Rng>> and std::same_as<iterator_t<Rng>, sentinel_t<Rng>>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::span<range_element_t<Rng>>, iterator_t<Rng>, iterator_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      const auto size = last - first;
      const auto count = std::min(static_cast<range_difference_t<Rng>>(n), size);
      return genex::span<range_element_t<Rng>>(first, first + (size - count));
    }

    template <typename Rng, typename Int>
      requires detail::concepts::droppable_last_range<Rng, Int>
      and std::bidirectional_iterator<iterator_t<Rng>> and std::same_as<iterator_t<Rng>, sentinel_t<Rng>>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<iterator_t<Rng>, sentinel_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      auto stop = iterators::prev(std::move(last), static_cast<range_difference_t<Rng>>(n), first);
      return genex::subrange<iterator_t<Rng>, iterator_t<Rng>>(std::move(first), std::move(stop));
    }

    template <typename Rng, typename Int>
      requires detail::concepts::droppable_last_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
      SAFE_CTOR(genex::subrange<iterator_t<Rng>, iterator_t<Rng>>, iterator_t<Rng>, iterator_t<Rng>) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      // See the iterator-pair overload: one traversal, not two.
      auto probe = iterators::next(first, static_cast<range_difference_t<Rng>>(n), last);
      auto stop = first;
      while (probe != last) {
        ++probe;
        ++stop;
      }
      return genex::subrange<iterator_t<Rng>, iterator_t<Rng>>(std::move(first), std::move(stop));
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
      SAFE_CTOR(drop_last_fn) and SAFE_MOVE(Int)) {
      return meta::bind_back(drop_last_fn{}, n);
    }
  };

  export inline constexpr drop_last_fn drop_last{};
}
