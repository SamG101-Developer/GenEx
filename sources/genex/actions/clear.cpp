module;
#include <genex/detail/macros.hpp>

export module genex.actions.clear;
export import genex.pipe;
import genex.concepts;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng>
  concept clearable_range =
    input_range<Rng> and
    has_member_clear<Rng>;
}

namespace genex::actions {
  struct clear_fn : pipe_closure {
    /// Overload for ranges that have a "clear" method on, simplifying chaining
    /// of different actions.
    template <typename Rng>
    requires detail::concepts::clearable_range<Rng>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const -> Rng {
      rng.clear();
      return std::forward<Rng>(rng);
    }
  };

  /// The "clear" action removes all elements from the range being applied to.
  /// Used as "range |= genex::actions::clear" or "genex:actions::clear(range)".
  /// The range itself must be an input range with a "clear" member.
  export inline constexpr clear_fn clear{};
}
