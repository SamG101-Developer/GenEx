module;
#include <genex/detail/macros.hpp>

export module genex.iterators.advance;
import genex.concepts;
import std;

namespace genex::iterators::detail::concepts {
  template <typename I>
  concept advancable_iters =
    std::input_or_output_iterator<I>;
}

namespace genex::iterators {
  struct advance_fn {
    template <typename I>
    requires detail::concepts::advancable_iters<I>
    GENEX_INLINE constexpr auto operator()(
      I &it, const iter_difference_t<I> n = 1) const -> I {
      GENEX_ASSERT(n >= 0);
      for (auto i = n; i > 0; --i) { ++it; }
      return it;
    }

    template <typename I>
    requires std::bidirectional_iterator<I>
    GENEX_INLINE constexpr auto operator()(
      I &it, const iter_difference_t<I> n = 1) const -> I {
      for (auto i = n; i < 0; ++i) { --it; }
      for (auto i = n; i > 0; --i) { ++it; }
      return it;
    }

    template <typename I>
    requires std::random_access_iterator<I>
    GENEX_INLINE constexpr auto operator()(
      I &it, const iter_difference_t<I> n = 1) const -> I {
      it += n;
      return it;
    }
  };

  export inline constexpr advance_fn advance{};
}
