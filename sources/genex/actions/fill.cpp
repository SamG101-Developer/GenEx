module;
#include <genex/macros.hpp>

export module genex.actions.fill;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename E>
  concept fillable_range =
    input_range<Rng> and
    std::assignable_from<range_reference_t<Rng>, const E&>;
}

namespace genex::actions {
  struct fill_fn {
    template <typename Rng, typename E>
      requires detail::concepts::fillable_range<Rng, E>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, E value) const -> decltype(auto) {
      auto [first, last] = iterators::iter_pair(rng);
      for (; first != last; ++first) { *first = value; }
      return std::forward<Rng>(rng);
    }

    template <typename E>
      requires (not range<E>)
    GENEX_INLINE constexpr auto operator()(E value) const {
      return meta::bind_back(fill_fn{}, std::move(value));
    }
  };

  export inline constexpr fill_fn fill{};
}