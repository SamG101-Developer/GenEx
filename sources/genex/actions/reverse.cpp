module;
#include <genex/detail/macros.hpp>

export module genex.actions.reverse;
export import genex.pipe;
import genex.algorithms.reverse;
import genex.concepts;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng>
  concept reversible_range =
    bidirectional_range<Rng> and
    permutable<iterator_t<Rng>>;
}

namespace genex::actions {
  struct reverse_fn : pipe_closure {
    template <typename Rng>
    requires detail::concepts::reversible_range<Rng>
    constexpr auto operator()(Rng &&rng) const -> Rng {
      genex::reverse(rng);
      return std::forward<Rng>(rng);
    }
  };

  export inline constexpr reverse_fn reverse{};
}
