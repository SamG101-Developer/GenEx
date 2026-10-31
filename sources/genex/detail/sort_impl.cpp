module;
#include <genex/detail/macros.hpp>

export module genex.detail.sort_impl;
import genex.algorithms.reverse;
import genex.concepts;
import genex.detail.partition_impl;
import genex.iterators.iter_swap;
import std;

export namespace genex::algorithms::detail {
  inline constexpr auto insertion_sort_limit = 24;
  inline constexpr auto ninther_limit = 128;
  inline constexpr auto partial_insertion_moves = 8;

  template <typename I, typename Less>
  constexpr auto insertion_sort(const I first, const I last, Less &less) -> void {
    if (first == last) { return; }
    for (auto current = first + 1; current != last; ++current) {
      if (less(*current, *(current - 1))) {
        iter_value_t<I> value(std::move(*current));
        auto hole = current;
        do {
          *hole = std::move(*(hole - 1));
          --hole;
        } while (hole != first and less(value, *(hole - 1)));
        *hole = std::move(value);
      }
    }
  }

  /// An insertion sort that needs no bounds check, as the element before
  /// @c first is known not to be greater than anything being sorted.
  template <typename I, typename Less>
  constexpr auto unguarded_insertion_sort(const I first, const I last, Less &less) -> void {
    if (first == last) { return; }
    for (auto current = first + 1; current != last; ++current) {
      if (less(*current, *(current - 1))) {
        iter_value_t<I> value(std::move(*current));
        auto hole = current;
        do {
          *hole = std::move(*(hole - 1));
          --hole;
        } while (less(value, *(hole - 1)));
        *hole = std::move(value);
      }
    }
  }

  /// An insertion sort that gives up after a few moves, for checking whether a
  /// range is almost sorted without spending much finding out it isn't.
  template <typename I, typename Less>
  constexpr auto partial_insertion_sort(const I first, const I last, Less &less) -> bool {
    if (first == last) { return true; }
    auto moves = iter_difference_t<I>{0};
    for (auto current = first + 1; current != last; ++current) {
      if (less(*current, *(current - 1))) {
        iter_value_t<I> value(std::move(*current));
        auto hole = current;
        do {
          *hole = std::move(*(hole - 1));
          --hole;
        } while (hole != first and less(value, *(hole - 1)));
        *hole = std::move(value);
        moves += current - hole;
      }
      if (moves > partial_insertion_moves) { return false; }
    }
    return true;
  }

  /// Merges two sorted halves from both ends at once, so it's two streams of
  /// comparisons that can run side by side, and neither needs a bounds check: over
  /// half the count from each end, a total order can't run out of either half.
  /// Each takes its element without a branch. Ties go to the left half going
  /// forwards and the right half going backwards, which keeps it stable.
  template <typename T, typename Out, typename Less>
  GENEX_INLINE constexpr auto merge_both_ends(
    T *const source, const std::ptrdiff_t count, Out out, Less &less) -> void {
    const auto half = count / 2;
    T *left = source;
    T *right = source + half;
    T *left_back = right - 1;
    T *right_back = source + (count - 1);
    auto out_back = out + static_cast<iter_difference_t<Out>>(count - 1);

    for (auto i = std::ptrdiff_t{0}; i < half; ++i) {
      const bool take_right = less(*right, *left);
      std::construct_at(std::addressof(*out), *(take_right ? right : left));
      right += take_right;
      left += not take_right;
      ++out;

      const bool take_left = less(*right_back, *left_back);
      std::construct_at(std::addressof(*out_back), *(take_left ? left_back : right_back));
      left_back -= take_left;
      right_back -= not take_left;
      --out_back;
    }

    if (count % 2 != 0) {
      const bool from_left = left <= left_back;
      std::construct_at(std::addressof(*out), *(from_left ? left : right));
    }
  }

  /// Sorts four elements into @c out, stably, with five comparisons and no
  /// branches on what they say.
  template <typename T, typename Less>
  GENEX_INLINE constexpr auto sort_four(T *const source, T *const out, Less &less) -> void {
    const bool first_pair = less(source[1], source[0]);
    const bool second_pair = less(source[3], source[2]);
    T *const a = source + first_pair;
    T *const b = source + static_cast<int>(not first_pair);
    T *const c = source + 2 + second_pair;
    T *const d = source + 2 + static_cast<int>(not second_pair);

    const bool c_first = less(*c, *a);
    const bool b_last = less(*d, *b);
    T *const lowest = c_first ? c : a;
    T *const highest = b_last ? b : d;
    T *const middle_left = c_first ? a : (b_last ? c : b);
    T *const middle_right = b_last ? d : (c_first ? b : c);

    const bool swapped = less(*middle_right, *middle_left);
    std::construct_at(out + 0, *lowest);
    std::construct_at(out + 1, *(swapped ? middle_right : middle_left));
    std::construct_at(out + 2, *(swapped ? middle_left : middle_right));
    std::construct_at(out + 3, *highest);
  }

  inline constexpr auto small_sort_limit = 32;

  /// Sorts up to @c small_sort_limit plain elements, stably. Each half starts from
  /// a run of four or eight built without branches, is extended by insertion, and
  /// the two are then merged back. Insertion on its own would mispredict on nearly
  /// every element of a small random range, and that's where most of a sort's
  /// branch misses are.
  template <typename I, typename Less>
  requires cheap_elements<I>
  auto small_sort(const I first, const I last, Less &less) -> void {
    using T = iter_value_t<I>;
    const auto count = static_cast<std::ptrdiff_t>(last - first);
    if (count < 8) {
      insertion_sort(first, last, less);
      return;
    }

    alignas(largest_cheap_element) std::byte input_storage[
      largest_cheap_element * small_sort_limit] [[indeterminate]];
    alignas(largest_cheap_element) std::byte scratch_storage[
      largest_cheap_element * small_sort_limit] [[indeterminate]];
    auto *const input = std::launder(reinterpret_cast<T*>(input_storage));
    auto *const scratch = std::launder(reinterpret_cast<T*>(scratch_storage));

    for (auto i = std::ptrdiff_t{0}; i < count; ++i) {
      std::construct_at(input + i, first[static_cast<iter_difference_t<I>>(i)]);
    }

    const auto half = count / 2;
    auto presorted = std::ptrdiff_t{4};
    if (count >= 16) {
      alignas(largest_cheap_element) std::byte runs_storage[largest_cheap_element * 8] [[indeterminate]];
      auto *const runs = std::launder(reinterpret_cast<T*>(runs_storage));
      sort_four(input, runs, less);
      sort_four(input + 4, runs + 4, less);
      merge_both_ends(runs, 8, scratch, less);
      sort_four(input + half, runs, less);
      sort_four(input + half + 4, runs + 4, less);
      merge_both_ends(runs, 8, scratch + half, less);
      presorted = 8;
    }
    else {
      sort_four(input, scratch, less);
      sort_four(input + half, scratch + half, less);
    }

    for (const auto offset : {std::ptrdiff_t{0}, half}) {
      const auto length = offset == 0 ? half : count - half;
      auto *const run = scratch + offset;
      for (auto i = presorted; i < length; ++i) {
        T value = input[offset + i];
        auto hole = i;
        while (hole > 0 and less(value, run[hole - 1])) {
          std::construct_at(run + hole, run[hole - 1]);
          --hole;
        }
        std::construct_at(run + hole, value);
      }
    }

    merge_both_ends(scratch, count, first, less);
  }

  template <typename I, typename Less>
  constexpr auto sift_down(
    const I first, const iter_difference_t<I> count, iter_difference_t<I> hole, Less &less) -> void {
    iter_value_t<I> value(std::move(first[hole]));
    while (true) {
      auto child = 2 * hole + 1;
      if (child >= count) { break; }
      if (child + 1 < count and less(first[child], first[child + 1])) { ++child; }
      if (not less(value, first[child])) { break; }
      first[hole] = std::move(first[child]);
      hole = child;
    }
    first[hole] = std::move(value);
  }

  template <typename I, typename Less>
  constexpr auto heap_sort(const I first, const I last, Less &less) -> void {
    const auto count = last - first;
    for (auto start = count / 2; start > 0; --start) {
      sift_down(first, count, start - 1, less);
    }
    for (auto end = count - 1; end > 0; --end) {
      iterators::iter_swap(first, first + end);
      sift_down(first, end, iter_difference_t<I>{0}, less);
    }
  }

  template <typename I, typename Less>
  GENEX_INLINE constexpr auto sort_three(const I a, const I b, const I c, Less &less) -> void {
    if (not less(*b, *a)) {
      if (not less(*c, *b)) { return; }
      iterators::iter_swap(b, c);
      if (less(*b, *a)) { iterators::iter_swap(a, b); }
      return;
    }
    if (less(*c, *b)) {
      iterators::iter_swap(a, c);
      return;
    }
    iterators::iter_swap(a, b);
    if (less(*c, *b)) { iterators::iter_swap(b, c); }
  }

  /// Handles a range that's already in order or strictly backwards, in one pass.
  /// A random range is found not to be one after about two comparisons. Reversing
  /// only strictly descending elements means that this is safe for a stable sort.
  template <typename I, typename Less>
  constexpr auto finish_if_one_run(const I first, const I last, Less &less) -> bool {
    if (last - first < 2) { return true; }

    auto current = first + 1;
    if (less(*current, *first)) {
      for (++current; current != last and less(*current, *(current - 1)); ++current) {}
      if (current != last) { return false; }
      genex::reverse(first, last);
      return true;
    }

    for (++current; current != last and not less(*current, *(current - 1)); ++current) {}
    return current == last;
  }

  template <typename I>
  struct partition_result {
    I pivot;
    bool already_partitioned;
  };

  /// Takes the pivot from @c first and puts everything less than it to the left,
  /// returning where it ended up.
  template <bool Branchless, typename I, typename Less>
  constexpr auto partition_right(const I first, const I last, Less &less) -> partition_result<I> {
    if constexpr (Branchless) {
      iter_value_t<I> pivot = *first;
      auto is_less = [&](auto &value) -> bool { return less(value, pivot); };
      const auto boundary = partition_branchless(first + 1, last, is_less);
      *first = *(boundary - 1);
      *(boundary - 1) = pivot;
      return {boundary - 1, false};
    }
    else {
      iter_value_t<I> pivot(std::move(*first));
      auto left = first;
      auto right = last;

      // The median of three leaves something on each side to stop these.
      while (less(*++left, pivot)) {}
      if (left - 1 == first) {
        while (left < right and not less(*--right, pivot)) {}
      }
      else {
        while (not less(*--right, pivot)) {}
      }

      const auto already_partitioned = left >= right;
      while (left < right) {
        iterators::iter_swap(left, right);
        while (less(*++left, pivot)) {}
        while (not less(*--right, pivot)) {}
      }

      const auto pivot_position = left - 1;
      *first = std::move(*pivot_position);
      *pivot_position = std::move(pivot);
      return {pivot_position, already_partitioned};
    }
  }

  /// Takes the pivot from @c first and puts everything not greater than it to the
  /// left. When the element before the range equals the pivot, those are all
  /// equal to it, so they're finished. This is what makes many duplicates linear.
  template <bool Branchless, typename I, typename Less>
  constexpr auto partition_left(const I first, const I last, Less &less) -> I {
    if constexpr (Branchless) {
      iter_value_t<I> pivot = *first;
      auto not_greater = [&](auto &value) -> bool { return not less(pivot, value); };
      const auto boundary = partition_branchless(first + 1, last, not_greater);
      *first = *(boundary - 1);
      *(boundary - 1) = pivot;
      return boundary - 1;
    }
    else {
      iter_value_t<I> pivot(std::move(*first));
      auto left = first;
      auto right = last;

      while (less(pivot, *--right)) {}
      if (right + 1 == last) {
        while (left < right and not less(pivot, *++left)) {}
      }
      else {
        while (not less(pivot, *++left)) {}
      }

      while (left < right) {
        iterators::iter_swap(left, right);
        while (less(pivot, *--right)) {}
        while (not less(pivot, *++left)) {}
      }

      *first = std::move(*right);
      *right = std::move(pivot);
      return right;
    }
  }

  /// Moves a few elements around to break up whatever pattern made a partition
  /// come out so uneven, so that the same choice of pivot doesn't happen again.
  template <typename I>
  constexpr auto break_patterns(const I first, const I last) -> void {
    const auto count = last - first;
    if (count < insertion_sort_limit) { return; }

    iterators::iter_swap(first, first + count / 4);
    iterators::iter_swap(last - 1, last - count / 4);
    if (count > ninther_limit) {
      iterators::iter_swap(first + 1, first + (count / 4 + 1));
      iterators::iter_swap(first + 2, first + (count / 4 + 2));
      iterators::iter_swap(last - 2, last - (count / 4 + 1));
      iterators::iter_swap(last - 3, last - (count / 4 + 2));
    }
  }

  /// A pattern-defeating quicksort, after Orson Peters' pdqsort, with the
  /// branchless partition from Lukas Bergdoll's ipnsort for plain elements. It
  /// recurses only into the smaller side, and falls back to a heap sort after too
  /// many uneven partitions, so it's always O(n log n).
  template <bool Branchless, typename I, typename Less>
  constexpr auto quicksort(
    I first, I last, Less &less, int bad_partitions_allowed, bool leftmost) -> void {
    while (true) {
      const auto count = last - first;
      if constexpr (Branchless) {
        if not consteval {
          if (count <= small_sort_limit) {
            small_sort(first, last, less);
            return;
          }
        }
      }
      if (count < insertion_sort_limit) {
        if (leftmost) { insertion_sort(first, last, less); }
        else { unguarded_insertion_sort(first, last, less); }
        return;
      }

      const auto half = count / 2;
      if (count > ninther_limit) {
        sort_three(first, first + half, last - 1, less);
        sort_three(first + 1, first + (half - 1), last - 2, less);
        sort_three(first + 2, first + (half + 1), last - 3, less);
        sort_three(first + (half - 1), first + half, first + (half + 1), less);
        iterators::iter_swap(first, first + half);
      }
      else {
        sort_three(first + half, first, last - 1, less);
      }

      if (not leftmost and not less(*(first - 1), *first)) {
        first = partition_left<Branchless>(first, last, less) + 1;
        continue;
      }

      const auto [pivot, already_partitioned] = partition_right<Branchless>(first, last, less);
      const auto left_count = pivot - first;
      const auto right_count = last - (pivot + 1);

      if (left_count < count / 8 or right_count < count / 8) {
        if (--bad_partitions_allowed == 0) {
          heap_sort(first, last, less);
          return;
        }
        break_patterns(first, pivot);
        break_patterns(pivot + 1, last);
      }
      else if (already_partitioned
        and partial_insertion_sort(first, pivot, less)
        and partial_insertion_sort(pivot + 1, last, less)) {
        return;
      }

      if (left_count < right_count) {
        quicksort<Branchless>(first, pivot, less, bad_partitions_allowed, leftmost);
        first = pivot + 1;
        leftmost = false;
      }
      else {
        quicksort<Branchless>(pivot + 1, last, less, bad_partitions_allowed, false);
        last = pivot;
      }
    }
  }

  inline constexpr auto order_samples = 64;

  /// Looks at a few neighbouring pairs for whether the input is mostly in order or
  /// mostly backwards. Comparisons on that are predictable, which is the one case
  /// where branches beat not having them, so it decides which way to partition.
  template <typename I, typename Less>
  constexpr auto looks_ordered(const I first, const I last, Less &less) -> bool {
    const auto count = last - first;
    if (count < 4 * small_sort_limit) { return false; }

    const auto stride = (count - 1) / order_samples;
    auto descents = 0;
    for (auto i = 0; i < order_samples; ++i) {
      const auto position = first + stride * i;
      descents += static_cast<int>(less(*(position + 1), *position));
    }
    return descents < order_samples / 8 or descents > order_samples - order_samples / 8;
  }

  template <typename I, typename Less>
  constexpr auto sort_any(const I first, const I last, Less &less) -> void {
    if (finish_if_one_run(first, last, less)) { return; }
    const auto bits = static_cast<int>(
      std::bit_width(static_cast<std::make_unsigned_t<iter_difference_t<I>>>(last - first)));

    if constexpr (cheap_elements<I>) {
      if (not looks_ordered(first, last, less)) {
        quicksort<true>(first, last, less, bits, true);
        return;
      }
    }
    quicksort<false>(first, last, less, bits, true);
  }
}
