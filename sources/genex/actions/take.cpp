module;
#include <genex/detail/macros.hpp>

export module genex.actions.take;
export import genex.pipe;
import genex.actions.erase;
import genex.concepts;
import genex.iterators.access;
import genex.iterators.next;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Int>
  concept takeable_range =
    forward_range<Rng> and
    std::integral<Int>;
}

namespace genex::actions {
  struct take_fn : pipe_closure {
    template <typename Rng, typename Int>
    requires detail::concepts::takeable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Int n) const -> Rng {
      GENEX_ASSERT(n >= 0);
      actions::erase(
        rng,
        iterators::next(iterators::begin(rng), static_cast<range_difference_t<Rng>>(n), iterators::end(rng)),
        iterators::end(rng));
      return std::forward<Rng>(rng);
    }

    template <typename Int>
    requires std::integral<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const {
      GENEX_ASSERT(n >= 0);
      return meta::bind_back(take_fn{}, n);
    }
  };

  export inline constexpr take_fn take{};
}
