module;
#include <genex/macros.hpp>

export module genex.views2.repeat;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import std;

namespace genex::views::detail::impl {
  struct repeat_sentinel {};

  template <typename T>
    requires std::copyable<T>
  struct repeat_iterator {
    T value;
    std::ptrdiff_t count = 0;

    using value_type = T;
    using reference_type = T;
    using reference = reference_type;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS_MINIMAL(repeat_iterator);

    GENEX_INLINE constexpr repeat_iterator() = default;

    GENEX_INLINE constexpr repeat_iterator(T value, const std::ptrdiff_t count) :
      value(std::move(value)), count(count) {
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      if (self.count > 0) { --self.count; }
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return self.value;
    }

    GENEX_VIEW_ITER_EQ(repeat_iterator, repeat_iterator) {
      return self.count == that.count;
    }

    GENEX_VIEW_ITER_EQ(repeat_iterator, repeat_sentinel) {
      GENEX_IGNORE(that);
      return self.count == 0;
    }
  };

  template <typename T>
    requires std::copyable<T>
  struct repeat_view {
    T value;
    std::ptrdiff_t count;

    GENEX_INLINE constexpr repeat_view(T value, const std::ptrdiff_t count) :
      value(std::move(value)), count(count) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return repeat_iterator<T>(self.value, self.count);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return repeat_sentinel();
    }
  };
}

namespace genex::views {
  struct repeat_fn {
    template <typename T, typename Int>
      requires std::copyable<T> and std::integral<Int>
    GENEX_INLINE constexpr auto operator()(T value, const Int count) const noexcept(
      SAFE_CTOR(detail::impl::repeat_view<T>, T, std::ptrdiff_t) and SAFE_MOVE(T)) {
      GENEX_ASSERT(std::out_of_range, count >= 0);
      return detail::impl::repeat_view<T>(std::move(value), static_cast<std::ptrdiff_t>(count));
    }

    template <typename T>
      requires std::copyable<T>
    GENEX_INLINE constexpr auto operator()(T value) const noexcept(
      SAFE_CTOR(detail::impl::repeat_view<T>, T, std::ptrdiff_t) and SAFE_MOVE(T)) {
      return detail::impl::repeat_view<T>(std::move(value), std::ptrdiff_t{-1});
    }
  };

  export inline constexpr repeat_fn repeat{};
}