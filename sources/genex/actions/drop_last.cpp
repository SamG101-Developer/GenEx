module;
#include <genex/detail/macros.hpp>

export module genex.actions.drop_last;
export import genex.pipe;
import genex.actions.erase;
import genex.concepts;
import genex.iterators.access;
import genex.iterators.distance;
import genex.iterators.next;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Int>
  concept droppable_last_range =
    forward_range<Rng> and
    has_member_erase<Rng> and
    std::integral<Int>;
}

namespace genex::actions {
  struct drop_last_fn : pipe_closure {
    template <typename Rng, typename Int>
    requires detail::concepts::droppable_last_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const -> Rng {
      GENEX_ASSERT(n >= 0);
      const auto len = iterators::distance(
        iterators::begin(rng), iterators::end(rng));
      const auto count = std::min(static_cast<std::ptrdiff_t>(n), len);
      const auto m = static_cast<range_difference_t<Rng>>(len - count);
      actions::erase(
        rng,
        iterators::next(iterators::begin(rng), m),
        iterators::end(rng));
      return std::forward<Rng>(rng);
    }

    template <typename Int>
    requires std::integral<Int>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(const Int n) const {
      GENEX_ASSERT(n >= 0);
      return meta::bind_back(drop_last_fn{}, n);
    }
  };

  export inline constexpr drop_last_fn drop_last{};
}