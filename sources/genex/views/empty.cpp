module;
#include <genex/detail/macros.hpp>

export module genex.views.empty;
export import genex.pipe;
import genex.concepts;
import std;

namespace genex::views::detail::impl {
  template <typename T>
  struct empty_view : view_base {
    [[nodiscard]] GENEX_INLINE constexpr auto begin() const noexcept -> const T* {
      return nullptr;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const noexcept -> const T* {
      return nullptr;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const noexcept -> std::size_t {
      return 0uz;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto empty() const noexcept -> bool {
      return true;
    }
  };
}

namespace genex::views {
  export template <typename T>
  inline constexpr detail::impl::empty_view<T> empty{};
}