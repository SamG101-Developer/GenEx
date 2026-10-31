module;
#include <genex/detail/macros.hpp>

export module genex.detail.libc_scan;
import genex.concepts;
import genex.meta;
import genex.operations.cmp;
import std;

export namespace genex::detail {
  /// A sized contiguous range of "T" that's read as itself, so its bytes can be
  /// handed straight to libc. A "volatile" element isn't one of these.
  template <typename I, typename S, typename T>
  concept libc_scannable_iters =
    std::contiguous_iterator<I> and
    std::sized_sentinel_for<S, I> and
    std::same_as<iter_value_t<I>, T> and
    (std::same_as<iter_reference_t<I>, T&> or
     std::same_as<iter_reference_t<I>, const T&>);

  /// The types libc can search for by value. "wmemchr" only helps where
  /// "wchar_t" is the same size as the type, which makes this 32-bit integers
  /// on Linux and macOS, and 16-bit integers on Windows.
  template <typename T>
  concept libc_findable_value =
    std::integral<T> and
    (sizeof(T) == 1 or
     (sizeof(T) == sizeof(wchar_t) and alignof(T) >= alignof(wchar_t)));

  /// The types where being equal and having equal bytes are the same thing.
  /// Floats aren't: 0.0 equals -0.0, and NaN doesn't equal itself. Neither are
  /// classes or enums, which can define an "operator==" of their own that
  /// doesn't look at every byte.
  template <typename T>
  concept libc_comparable_value = std::integral<T> or std::is_pointer_v<T>;

  /// The first element equal to "value", or "first + n" if there isn't one.
  template <libc_findable_value T>
  [[nodiscard]] GENEX_INLINE auto libc_find(
    const T *const first, const std::size_t n, const T value) noexcept -> const T* {
    if (n == 0) { return first; }
    if constexpr (sizeof(T) == 1) {
      const void *const found = std::memchr(
        first, static_cast<int>(static_cast<unsigned char>(value)), n);
      return found != nullptr ? static_cast<const T*>(found) : first + n;
    }
    else {
      const wchar_t *const found = std::wmemchr(
        reinterpret_cast<const wchar_t*>(first), static_cast<wchar_t>(value), n);
      return found != nullptr ? reinterpret_cast<const T*>(found) : first + n;
    }
  }

  template <libc_comparable_value T>
  [[nodiscard]] GENEX_INLINE auto libc_equal(
    const T *const a, const T *const b, const std::size_t n) noexcept -> bool {
    return n == 0 or std::memcmp(a, b, n * sizeof(T)) == 0;
  }

  /// Whether searching for an "E" in these iterators gives the same answer from
  /// libc as it would from comparing each element to it. Only a value of exactly
  /// the element's type counts, so a value too wide for it can't be cut down to
  /// one that matches.
  template <typename I, typename S, typename E, typename Proj>
  concept libc_find_usable =
    std::same_as<Proj, meta::identity> and
    libc_findable_value<std::remove_cvref_t<E>> and
    libc_scannable_iters<I, S, std::remove_cvref_t<E>>;

  /// Whether comparing these ranges gives the same answer from libc as it would
  /// from comparing them an element at a time.
  template <
    typename I1, typename S1, typename I2, typename S2, typename Comp,
    typename Proj1, typename Proj2>
  concept libc_equal_usable =
    std::same_as<Comp, operations::eq> and
    std::same_as<Proj1, meta::identity> and
    std::same_as<Proj2, meta::identity> and
    libc_comparable_value<iter_value_t<I1>> and
    libc_scannable_iters<I1, S1, iter_value_t<I1>> and
    libc_scannable_iters<I2, S2, iter_value_t<I1>>;

  /// The first element equal to "value", or the end if there isn't one.
  template <typename I, typename S, typename T>
  [[nodiscard]] GENEX_INLINE auto libc_find_iters(
    I first, const S last, const T value) -> I {
    const T *const data = std::to_address(first);
    const auto n = static_cast<std::size_t>(last - first);
    const T *const found = libc_find<T>(data, n, value);
    return first + static_cast<iter_difference_t<I>>(found - data);
  }

  template <typename I1, typename S1, typename I2, typename S2>
  [[nodiscard]] GENEX_INLINE auto libc_equal_iters(
    const I1 first1, const S1 last1, const I2 first2, const S2 last2) -> bool {
    const auto n = last1 - first1;
    return n == last2 - first2 and libc_equal<iter_value_t<I1>>(
      std::to_address(first1), std::to_address(first2),
      static_cast<std::size_t>(n));
  }
}
