module;
#include <genex/detail/macros.hpp>

export module genex.iterators.iter_pair;
import genex.concepts;
import genex.iterators.access;
import std;

namespace genex::iterators::detail::concepts {
  template <typename Rng>
  concept iter_pairable_range =
    input_range<Rng>;
}

namespace genex::iterators {
  struct iter_pair_fn {
    template <typename Rng>
    requires detail::concepts::iter_pairable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::begin), Rng&) and
      SAFE_CALL(decltype(iterators::end), Rng&) and
      SAFE_MOVE(iterator_t<Rng&>) and SAFE_MOVE(sentinel_t<Rng&>)) {
      return std::make_pair(iterators::begin(rng), iterators::end(rng));
    }
  };

  export inline constexpr iter_pair_fn iter_pair{};
}
