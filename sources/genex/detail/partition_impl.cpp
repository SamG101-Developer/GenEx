module;
#include <genex/detail/macros.hpp>

export module genex.detail.partition_impl;
import genex.concepts;
import genex.iterators.iter_swap;
import genex.meta;
import std;

export namespace genex::algorithms::detail {
  inline constexpr auto largest_cheap_element = 2 * sizeof(void*);

  /// Whether elements are plain enough that it pays to always write them, so
  /// that where they go doesn't depend on a branch. A branch that goes either
  /// way at random is wrong half the time, and that costs far more than these
  /// writes do. It has to be a real reference to do this, not a proxy. Trivial
  /// assignment isn't needed, which lets in a @c std::pair of plain things.
  template <typename I>
  concept cheap_elements =
    std::random_access_iterator<I> and
    std::is_lvalue_reference_v<iter_reference_t<I>> and
    std::same_as<std::remove_cvref_t<iter_reference_t<I>>, iter_value_t<I>> and
    std::is_trivially_copy_constructible_v<iter_value_t<I>> and
    std::is_trivially_destructible_v<iter_value_t<I>> and
    std::is_nothrow_copy_assignable_v<iter_value_t<I>> and
    sizeof(iter_value_t<I>) <= largest_cheap_element;

  /// Moves a gap through the range. Each element goes to the boundary, the one
  /// at the boundary goes to the gap, and only the boundary's position depends
  /// on the predicate, without a branch.
  template <typename I, typename Pred>
  requires cheap_elements<I>
  constexpr auto partition_branchless(I first, const I last, Pred &pred) -> I {
    if (first == last) { return first; }

    iter_value_t<I> gap_value = *first;
    auto gap = first;
    auto boundary = first;
    for (auto right = first + 1; right != last; ++right) {
      const bool goes_left = meta::invoke(pred, *right);
      *gap = *boundary;
      *boundary = *right;
      boundary += goes_left;
      gap = right;
    }

    *gap = *boundary;
    *boundary = gap_value;
    boundary += static_cast<bool>(meta::invoke(pred, gap_value));
    return boundary;
  }

  template <typename I, typename Pred>
  requires std::bidirectional_iterator<I>
  constexpr auto partition_from_both_ends(I first, I last, Pred &pred) -> I {
    while (true) {
      while (true) {
        if (first == last) { return first; }
        if (not meta::invoke(pred, *first)) { break; }
        ++first;
      }
      do {
        --last;
        if (first == last) { return first; }
      } while (not meta::invoke(pred, *last));
      iterators::iter_swap(first, last);
      ++first;
    }
  }

  template <typename I, typename Pred>
  constexpr auto partition_forward(I first, const I last, Pred &pred) -> I {
    while (true) {
      if (first == last) { return first; }
      if (not meta::invoke(pred, *first)) { break; }
      ++first;
    }

    auto next = first;
    for (++next; next != last; ++next) {
      if (meta::invoke(pred, *next)) {
        iterators::iter_swap(first, next);
        ++first;
      }
    }
    return first;
  }

  template <typename I, typename Pred>
  GENEX_INLINE constexpr auto partition_any(I first, I last, Pred &pred) -> I {
    if constexpr (cheap_elements<I>) {
      return partition_branchless(std::move(first), std::move(last), pred);
    }
    else if constexpr (std::bidirectional_iterator<I>) {
      return partition_from_both_ends(std::move(first), std::move(last), pred);
    }
    else {
      return partition_forward(std::move(first), std::move(last), pred);
    }
  }
}
