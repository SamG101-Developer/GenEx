module;
#include <genex/detail/macros.hpp>

export module genex.detail.stable_partition_impl;
import genex.algorithms.rotate;
import genex.concepts;
import genex.detail.partition_impl;
import genex.detail.temporary_buffer;
import genex.iterators.next;
import genex.meta;
import std;

export namespace genex::algorithms::detail {
  /// Partitions each half, then swaps the second half's matches with the first
  /// half's rest. That's O(n log n) moves rather than O(n), but needs no memory.
  template <typename I, typename Pred>
  constexpr auto stable_partition_in_place(
    I first, I last, Pred &pred, const iter_difference_t<I> count) -> I {
    if (count == 0) { return first; }
    if (count == 1) {
      return meta::invoke(pred, *first) ? iterators::next(first) : first;
    }

    const auto half = count / 2;
    auto mid = iterators::next(first, half);
    auto left = stable_partition_in_place(first, mid, pred, half);
    auto right = stable_partition_in_place(mid, last, pred, count - half);
    return genex::rotate(std::move(left), std::move(mid), std::move(right));
  }

  // Matches slide forward in the range, and the rest wait in the buffer. The first
  // element is already known not to match.
  template <typename I, typename Pred>
  constexpr auto stable_partition_buffered(
    I first, const I last, Pred &pred, genex::detail::temporary_buffer<iter_value_t<I>> &buffer) -> I {
    using T = iter_value_t<I>;
    auto it = iterators::next(first);

    if constexpr (cheap_elements<I>) {
      if not consteval {
        // Every element is written to both places, and only where the next one
        // goes depends on the predicate, so there's no branch to mispredict.
        auto *const rest = buffer.data();
        std::construct_at(rest, *first);
        auto rest_count = 1uz;
        for (; it != last; ++it) {
          T value = *it;
          const bool matches = meta::invoke(pred, value);
          *first = value;
          std::construct_at(rest + rest_count, value);
          first += matches;
          rest_count += not matches;
        }

        auto out = first;
        for (auto i = 0uz; i < rest_count; ++i, ++out) { *out = rest[i]; }
        return first;
      }
    }

    buffer.push_back(std::move(*first));
    for (; it != last; ++it) {
      if (meta::invoke(pred, *it)) {
        *first = std::move(*it);
        ++first;
      }
      else {
        buffer.push_back(std::move(*it));
      }
    }

    auto out = first;
    for (auto i = 0uz; i < buffer.size(); ++i, ++out) {
      *out = std::move(buffer.data()[i]);
    }
    buffer.clear();
    return first;
  }

  template <typename I, typename Pred>
  constexpr auto stable_partition_any(I first, I last, Pred &pred) -> I {
    while (true) {
      if (first == last) { return first; }
      if (not meta::invoke(pred, *first)) { break; }
      ++first;
    }

    auto count = iter_difference_t<I>{0};
    for (auto it = first; it != last; ++it) { ++count; }

    auto buffer = genex::detail::temporary_buffer<iter_value_t<I>>(static_cast<std::size_t>(count));
    if (buffer.capacity() == 0) {
      auto next = iterators::next(first);
      auto right = stable_partition_in_place(next, std::move(last), pred, count - 1);
      return genex::rotate(std::move(first), std::move(next), std::move(right));
    }
    return stable_partition_buffered(std::move(first), std::move(last), pred, buffer);
  }
}
