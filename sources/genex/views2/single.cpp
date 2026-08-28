module;
#include <genex/macros.hpp>

export module genex.views2.single;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.views2.repeat;
import std;

namespace genex::views {
  struct single_fn {
    template <typename T>
      requires std::copyable<T>
    GENEX_INLINE constexpr auto operator()(T value) const noexcept(
      SAFE_CALL(decltype(repeat), T, std::ptrdiff_t) and SAFE_MOVE(T)) {
      return repeat(std::move(value), std::ptrdiff_t{1});
    }
  };

  export inline constexpr single_fn single{};
}