module;
#include <genex/detail/macros.hpp>

export module genex.detail.stable_sort_impl;
import genex.algorithms.rotate;
import genex.concepts;
import genex.detail.partition_impl;
import genex.detail.sort_impl;
import genex.detail.temporary_buffer;
import genex.iterators.iter_swap;
import std;

export namespace genex::algorithms::detail {
  inline constexpr auto stable_insertion_sort_limit = 20;

  // These take the element as it's stored, so that a comparison taking non-const
  // references can be used, which going through a search taking a key can't do.
  template <typename I, typename Less>
  constexpr auto first_not_less(I first, I last, iter_reference_t<I> value, Less &less) -> I {
    auto count = last - first;
    while (count > 0) {
      const auto half = count / 2;
      const auto mid = first + half;
      if (less(*mid, value)) {
        first = mid + 1;
        count -= half + 1;
      }
      else {
        count = half;
      }
    }
    return first;
  }

  template <typename I, typename Less>
  constexpr auto first_greater(I first, I last, iter_reference_t<I> value, Less &less) -> I {
    auto count = last - first;
    while (count > 0) {
      const auto half = count / 2;
      const auto mid = first + half;
      if (not less(value, *mid)) {
        first = mid + 1;
        count -= half + 1;
      }
      else {
        count = half;
      }
    }
    return first;
  }

  /// Merges without any memory. The middle of the longer side splits both sides,
  /// and a rotate puts those parts on the correct side of each other, leaving two
  /// smaller merges. That's O(n log n) for each merge, compared to O(n) with one.
  /// It only recurses into the first, so the depth stays logarithmic.
  template <typename I, typename Less>
  constexpr auto merge_in_place(
    I first, I mid, const I last,
    iter_difference_t<I> left_count, iter_difference_t<I> right_count, Less &less) -> void {
    while (left_count != 0 and right_count != 0) {
      if (left_count + right_count == 2) {
        if (less(*mid, *first)) { iterators::iter_swap(first, mid); }
        return;
      }

      auto left_cut = first;
      auto right_cut = mid;
      auto left_part = iter_difference_t<I>{0};
      auto right_part = iter_difference_t<I>{0};
      if (left_count >= right_count) {
        left_part = left_count / 2;
        left_cut = first + left_part;
        right_cut = first_not_less(mid, last, *left_cut, less);
        right_part = right_cut - mid;
      }
      else {
        right_part = right_count / 2;
        right_cut = mid + right_part;
        left_cut = first_greater(first, mid, *right_cut, less);
        left_part = left_cut - first;
      }

      const auto new_mid = genex::rotate(left_cut, mid, right_cut);
      merge_in_place(first, left_cut, new_mid, left_part, right_part, less);

      first = new_mid;
      mid = right_cut;
      left_count -= left_part;
      right_count -= right_part;
    }
  }

  template <typename I, typename Less>
  constexpr auto stable_sort_in_place(const I first, const I last, Less &less) -> void {
    const auto count = last - first;
    if (count <= stable_insertion_sort_limit) {
      insertion_sort(first, last, less);
      return;
    }

    const auto half = count / 2;
    const auto mid = first + half;
    stable_sort_in_place(first, mid, less);
    stable_sort_in_place(mid, last, less);
    merge_in_place(first, mid, last, half, count - half, less);
  }

  /// Moves the left side out into the buffer and merges forwards, which can't
  /// catch up with the right side that's still to be read. On a tie the left side
  /// goes first, which is what keeps it stable.
  template <typename I, typename Less>
  constexpr auto merge_buffered(
    const I first, const I mid, const I last, Less &less, iter_value_t<I> *const buffer) -> void {
    if (not less(*mid, *(mid - 1))) { return; }

    auto *left_end = buffer;
    for (auto it = first; it != mid; ++it, ++left_end) { *left_end = std::move(*it); }

    auto *left = buffer;
    auto right = mid;
    auto out = first;
    while (left != left_end and right != last) {
      if (less(*right, *left)) {
        *out = std::move(*right);
        ++right;
      }
      else {
        *out = std::move(*left);
        ++left;
      }
      ++out;
    }
    for (; left != left_end; ++left, ++out) { *out = std::move(*left); }
  }

  /// For plain elements, sorting up to @c small_sort_limit and merging from both
  /// ends, all without branching on the comparisons.
  template <typename I, typename Less>
  requires cheap_elements<I>
  auto stable_sort_branchless(
    const I first, const I last, Less &less, iter_value_t<I> *const buffer) -> void {
    const auto count = last - first;
    if (count <= small_sort_limit) {
      small_sort(first, last, less);
      return;
    }

    const auto half = count / 2;
    const auto mid = first + half;
    stable_sort_branchless(first, mid, less, buffer);
    stable_sort_branchless(mid, last, less, buffer);
    if (not less(*mid, *(mid - 1))) { return; }

    for (auto i = iter_difference_t<I>{0}; i < count; ++i) { buffer[i] = first[i]; }
    merge_both_ends(buffer, count, first, less);
  }

  template <typename I, typename Less>
  constexpr auto stable_sort_buffered(
    const I first, const I last, Less &less, iter_value_t<I> *const buffer) -> void {
    const auto count = last - first;
    if (count <= stable_insertion_sort_limit) {
      insertion_sort(first, last, less);
      return;
    }

    const auto mid = first + count / 2;
    stable_sort_buffered(first, mid, less, buffer);
    stable_sort_buffered(mid, last, less, buffer);
    merge_buffered(first, mid, last, less, buffer);
  }

  template <typename I, typename Less>
  constexpr auto stable_sort_any(const I first, const I last, Less &less) -> void {
    if (finish_if_one_run(first, last, less)) { return; }

    if constexpr (cheap_elements<I>) {
      if not consteval {
        auto buffer = genex::detail::temporary_buffer<iter_value_t<I>>(static_cast<std::size_t>(last - first));
        if (buffer.capacity() != 0) {
          stable_sort_branchless(first, last, less, buffer.data());
          return;
        }
      }
    }

    auto buffer = genex::detail::temporary_buffer<iter_value_t<I>>(static_cast<std::size_t>((last - first + 1) / 2));
    if (buffer.capacity() == 0) {
      stable_sort_in_place(first, last, less);
      return;
    }

    iter_value_t<I> seed(std::move(*first));
    buffer.fill_from(seed);
    *first = std::move(seed);
    stable_sort_buffered(first, last, less, buffer.data());
  }
}
