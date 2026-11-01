module;
#include <genex/detail/macros.hpp>

export module genex.iterators.iter_swap;
import genex.concepts;
import std;

namespace genex::iterators {
  struct iter_swap_fn {
    template <typename I1, typename I2>
    requires std::indirectly_swappable<I1, I2>
    GENEX_INLINE constexpr auto operator()(
      const I1 &a, const I2 &b) const noexcept(
      SAFE_NOEX(*a) and SAFE_NOEX(*b) and
      SAFE_SWAP(decltype(*a), decltype(*b))) -> void {
      using std::swap;
      swap(*a, *b);
    }
  };

  export inline constexpr iter_swap_fn iter_swap{};
}
