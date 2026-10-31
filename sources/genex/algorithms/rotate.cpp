module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.rotate;
import genex.algorithms.reverse;
import genex.concepts;
import genex.iterators.access;
import genex.iterators.iter_swap;
import genex.iterators.next;
import std;

namespace genex::algorithms::detail::concepts {
  template <typename I, typename S>
  concept rotatable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    permutable<I>;

  template <typename Rng>
  concept rotatable_range =
    forward_range<Rng> and
    rotatable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::algorithms::detail::impl {
  inline constexpr auto stack_buffer_bytes = 256uz;

  template <typename I>
  concept bytewise_rotatable =
    std::contiguous_iterator<I> and
    std::is_trivially_copyable_v<iter_value_t<I>>;

  /// Swaps whichever of the two blocks is smaller into place, which leaves the
  /// same problem on what's left. Unlike following the cycles of a rotation,
  /// this walks through memory in order, and each swapping loop can be
  /// vectorized.
  template <typename I>
  requires std::random_access_iterator<I>
  constexpr auto rotate_by_blocks(I first, I mid, I last) -> I {
    using diff_t = iter_difference_t<I>;
    auto left = mid - first;
    auto right = last - mid;
    const auto result = first + right;

    while (left != 0 and right != 0) {
      if (left <= right) {
        for (auto i = diff_t{0}; i < left; ++i) {
          iterators::iter_swap(first + i, mid + i);
        }
        first += left;
        mid += left;
        right -= left;
      }
      else {
        const auto start = mid - right;
        for (auto i = diff_t{0}; i < right; ++i) {
          iterators::iter_swap(start + i, mid + i);
        }
        last -= right;
        mid -= right;
        left -= right;
      }
    }
    return result;
  }

  template <typename I>
  requires std::bidirectional_iterator<I>
  constexpr auto rotate_by_reversing(I first, I mid, I last) -> I {
    genex::reverse(first, mid);
    genex::reverse(mid, last);
    while (first != mid and mid != last) {
      --last;
      iterators::iter_swap(first, last);
      ++first;
    }
    if (first == mid) {
      genex::reverse(mid, last);
      return last;
    }
    genex::reverse(first, mid);
    return first;
  }

  template <typename I>
  constexpr auto rotate_forward(I first, I mid, const I last) -> I {
    auto next = mid;
    do {
      iterators::iter_swap(first, next);
      ++first;
      ++next;
      if (first == mid) { mid = next; }
    } while (next != last);

    const auto result = first;
    next = mid;
    while (next != last) {
      iterators::iter_swap(first, next);
      ++first;
      ++next;
      if (first == mid) { mid = next; }
      else if (next == last) { next = mid; }
    }
    return result;
  }

  template <typename I>
  constexpr auto do_rotate(I first, I mid, I last) -> I {
    if (first == mid) { return last; }
    if (mid == last) { return first; }

    if constexpr (bytewise_rotatable<I>) {
      if not consteval {
        // When one side fits on the stack, then the other can be slid along it
        // in one go. This covers the common case of rotating by a little.
        using T = iter_value_t<I>;
        const auto left = static_cast<std::size_t>(mid - first);
        const auto right = static_cast<std::size_t>(last - mid);
        auto *const begin_ptr = std::to_address(first);
        auto *const mid_ptr = std::to_address(mid);

        if (left <= right and left * sizeof(T) <= stack_buffer_bytes) {
          std::byte stack[stack_buffer_bytes] [[indeterminate]];
          std::memcpy(stack, begin_ptr, left * sizeof(T));
          std::memmove(begin_ptr, mid_ptr, right * sizeof(T));
          std::memcpy(begin_ptr + right, stack, left * sizeof(T));
          return first + static_cast<iter_difference_t<I>>(right);
        }
        if (right < left and right * sizeof(T) <= stack_buffer_bytes) {
          std::byte stack[stack_buffer_bytes] [[indeterminate]];
          std::memcpy(stack, mid_ptr, right * sizeof(T));
          std::memmove(begin_ptr + right, begin_ptr, left * sizeof(T));
          std::memcpy(begin_ptr, stack, right * sizeof(T));
          return first + static_cast<iter_difference_t<I>>(right);
        }
      }
    }

    if constexpr (std::random_access_iterator<I>) {
      return rotate_by_blocks(std::move(first), std::move(mid), std::move(last));
    }
    else if constexpr (std::bidirectional_iterator<I>) {
      return rotate_by_reversing(std::move(first), std::move(mid), std::move(last));
    }
    else {
      return rotate_forward(std::move(first), std::move(mid), std::move(last));
    }
  }
}

namespace genex {
  struct rotate_fn {
    template <typename I, typename S>
    requires algorithms::detail::concepts::rotatable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, I mid, S last) const -> I {
      auto end = iterators::next(mid, std::move(last));
      return algorithms::detail::impl::do_rotate(
        std::move(first), std::move(mid), std::move(end));
    }

    template <typename Rng>
    requires algorithms::detail::concepts::rotatable_range<Rng>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, iterator_t<Rng> mid) const -> borrowed_iterator_t<Rng> {
      auto first = iterators::begin(rng);
      auto last = iterators::end(rng);
      return (*this)(std::move(first), std::move(mid), std::move(last));
    }
  };

  export inline constexpr rotate_fn rotate{};
}
