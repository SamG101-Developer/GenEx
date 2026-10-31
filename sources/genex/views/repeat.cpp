module;
#include <genex/detail/macros.hpp>

export module genex.views.repeat;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import std;

namespace genex::views::detail::impl {
  struct repeat_sentinel {};

  template <typename T, bool Counted>
  requires std::copyable<T>
  struct repeat_iterator :
    genex::detail::iterator_ops<repeat_iterator<T, Counted>> {
    T value;
    std::ptrdiff_t count = 0;

    using value_type = T;
    using reference = T;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr repeat_iterator() = default;

    GENEX_INLINE constexpr repeat_iterator(T value, const std::ptrdiff_t count) :
      value(std::move(value)), count(count) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      if constexpr (Counted) { --count; }
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return value;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const repeat_iterator &self, const repeat_iterator &that) -> bool {
      return self.count == that.count;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const repeat_iterator &self, repeat_sentinel) -> bool {
      return self.count == 0;
    }
  };

  template <typename T, bool Counted>
  requires std::copyable<T>
  struct repeat_view : view_base {
    T value;
    std::ptrdiff_t count;

    GENEX_INLINE constexpr repeat_view(
      T value, const std::ptrdiff_t count) noexcept(
      SAFE_MOVE(T)) :
      value(std::move(value)), count(count) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return repeat_iterator<T, Counted>(value, count);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      if constexpr (Counted) { return repeat_sentinel(); }
      else { return unreachable_sentinel(); }
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const -> std::size_t
    requires Counted {
      return static_cast<std::size_t>(count);
    }
  };
}

namespace genex::views {
  struct repeat_fn : pipe_closure {
    template <typename T, typename Int>
    requires std::copyable<T> and std::integral<Int>
    GENEX_INLINE constexpr auto operator()(
      T value, const Int count) const noexcept(
      SAFE_CTOR(detail::impl::repeat_view<T, true>, T, std::ptrdiff_t) and
      SAFE_MOVE(T)) {
      GENEX_ASSERT(count >= 0);
      return detail::impl::repeat_view<T, true>(
        std::move(value), static_cast<std::ptrdiff_t>(count));
    }

    template <typename T>
    requires std::copyable<T>
    GENEX_INLINE constexpr auto operator()(
      T value) const noexcept(
      SAFE_CTOR(detail::impl::repeat_view<T, false>, T, std::ptrdiff_t) and
      SAFE_MOVE(T)) {
      return detail::impl::repeat_view<T, false>(
        std::move(value), std::ptrdiff_t{0});
    }
  };

  export inline constexpr repeat_fn repeat{};
}