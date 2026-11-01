module;
#include <genex/detail/macros.hpp>

export module genex.detail.counted_iterator;
import genex.concepts;
import genex.detail.iterator_ops;
import std;

export namespace genex::detail {
  /// Walks an underlying iterator while counting down how many elements are
  /// left to it. Two of these are compared and subtracted by that count alone,
  /// so they're sized whatever the underlying iterator is, but only two that
  /// started from the same place can be compared.
  template <typename I>
  requires std::input_iterator<I>
  struct counted_iterator :
    iterator_ops<counted_iterator<I>> {
    I it;
    iter_difference_t<I> n = 0;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_concept = std::conditional_t<
      std::contiguous_iterator<I>, std::contiguous_iterator_tag, iter_strength_t<I>>;
    using iterator_category = legacy_iter_category_t<
      reference, iter_strength_t<I>>;

    GENEX_INLINE constexpr counted_iterator() = default;

    GENEX_INLINE constexpr counted_iterator(
      I first, const difference_type count) noexcept(
      SAFE_MOVE(I)) :
      it(std::move(first)), n(count) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto base() const noexcept -> const I& {
      return it;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto count() const noexcept -> difference_type {
      return n;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *it;
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      --n;
    }

    GENEX_INLINE constexpr auto prev() -> void
    requires std::bidirectional_iterator<I> {
      --it;
      ++n;
    }

    GENEX_INLINE constexpr auto advance_by(
      const difference_type m) -> void
    requires std::random_access_iterator<I> {
      it += m;
      n -= m;
    }

    GENEX_INLINE constexpr auto distance_to(
      const counted_iterator &that) const noexcept -> difference_type {
      return n - that.n;
    }

    GENEX_INLINE constexpr auto operator->() const
    requires std::contiguous_iterator<I> {
      return std::to_address(it);
    }

    GENEX_INLINE friend constexpr auto operator==(
      const counted_iterator &self,
      const counted_iterator &that) noexcept -> bool {
      return self.n == that.n;
    }
  };

  /// Ends a "counted_iterator" when its count runs out or it reaches "st",
  /// whichever comes first. Use "unreachable_sentinel" to only go by the count.
  template <typename S>
  struct counted_sentinel {
    GENEX_NO_UNIQUE_ADDRESS S st;

    template <typename I>
    GENEX_INLINE friend constexpr auto operator==(
      const counted_iterator<I> &it, const counted_sentinel &self) -> bool {
      return it.count() == 0 or it.base() == self.st;
    }

    template <typename I>
    requires std::sized_sentinel_for<S, I>
      or std::same_as<S, unreachable_sentinel>
    GENEX_INLINE friend constexpr auto operator-(
      const counted_sentinel &self,
      const counted_iterator<I> &it) -> iter_difference_t<I> {
      if constexpr (std::same_as<S, unreachable_sentinel>) { return it.count(); }
      else { return std::min(it.count(), self.st - it.base()); }
    }

    template <typename I>
    requires std::sized_sentinel_for<S, I>
      or std::same_as<S, unreachable_sentinel>
    GENEX_INLINE friend constexpr auto operator-(
      const counted_iterator<I> &it,
      const counted_sentinel &self) -> iter_difference_t<I> {
      return -(self - it);
    }
  };
}
