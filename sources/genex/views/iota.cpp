module;
#include <genex/detail/macros.hpp>

export module genex.views.iota;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import std;

namespace genex::views::detail::impl {
  template <typename Int>
    requires std::weakly_incrementable<Int>
  struct iota_iterator : genex::detail::iterator_ops<iota_iterator<Int>> {
    Int lo;
    Int hi;
    Int step;
    Int it;

    using value_type = Int;
    using reference = Int;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept = iterator_category;

    using pointer = void;

    GENEX_INLINE constexpr iota_iterator() = default;

    GENEX_INLINE constexpr iota_iterator(const Int lo, const Int hi, const Int step)
      : lo(lo), hi(hi), step(step), it(lo) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      it += step;
    }

    GENEX_INLINE constexpr auto prev() -> void {
      it -= step;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return it;
    }

    GENEX_INLINE constexpr auto advance_by(const difference_type n) -> void requires std::integral<Int> {
      it = static_cast<Int>(static_cast<difference_type>(it) + n * static_cast<difference_type>(step));
    }

    GENEX_INLINE constexpr auto distance_to(const iota_iterator &that) const -> difference_type
      requires std::integral<Int> {
      const auto gap = static_cast<difference_type>(that.it) - static_cast<difference_type>(it);
      const auto width = static_cast<difference_type>(step);
      return gap >= 0 ? (gap + width - 1) / width : -((-gap + width - 1) / width);
    }

    // Every position past the end is the end, as the last step can overshoot it.
    GENEX_INLINE friend constexpr auto operator==(const iota_iterator &self, const iota_iterator &that) -> bool {
      return self.it == that.it or (self.it >= self.hi and that.it >= that.hi);
    }
  };

  template <typename Int>
    requires std::weakly_incrementable<Int>
  struct iota_view : view_base {
    Int lo;
    Int hi;
    Int step;

    GENEX_INLINE constexpr iota_view(const Int lo, const Int hi, const Int step) noexcept(
      SAFE_CTOR(Int, const Int&))
      : lo(lo), hi(hi), step(step) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return iota_iterator<Int>(lo, hi, step);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return iota_iterator<Int>(hi, hi, step);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires std::integral<Int> {
      return (hi - lo + step - 1) / step;
    }
  };
}

namespace genex::views {
  struct iota_fn {
    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int lo, const Int hi, const Int step = 1) const noexcept(
      SAFE_CTOR(detail::impl::iota_view<Int>, Int, Int, Int) and
      SAFE_MOVE(Int)) {
      return detail::impl::iota_view(lo, hi, step);
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int hi) const noexcept(
      SAFE_CTOR(detail::impl::iota_view<Int>, Int, Int, Int) and
      SAFE_MOVE(Int)) {
      return detail::impl::iota_view(static_cast<Int>(0), hi, static_cast<Int>(1));
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
      and std::numeric_limits<Int>::is_specialized
    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(detail::impl::iota_view<Int>, Int, Int, Int) and
      SAFE_MOVE(Int)) {
      return detail::impl::iota_view(static_cast<Int>(0), std::numeric_limits<Int>::max(), static_cast<Int>(1));
    }
  };

  export inline constexpr iota_fn iota{};
}
