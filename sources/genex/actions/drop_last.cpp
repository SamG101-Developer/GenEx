module;
#include <genex/macros.hpp>

export module genex.actions.drop_last;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.actions.erase;
import genex.iterators.access;
import genex.iterators.distance;
import genex.iterators.next;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Int>
  concept droppable_last_range =
    forward_range<Rng> and
    has_member_erase<Rng> and
    std::weakly_incrementable<Int>;
}

namespace genex::actions {
  struct drop_last_fn {
    template <typename Rng, typename Int>
      requires detail::concepts::droppable_last_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const -> decltype(auto) {
      GENEX_ASSERT(std::out_of_range, n >= 0);
      const auto len = iterators::distance(iterators::begin(rng), iterators::end(rng));
      const auto count = std::min(static_cast<std::ptrdiff_t>(n), len);
      actions::erase(
        rng,
        iterators::next(iterators::begin(rng), static_cast<range_difference_t<Rng>>(len - count)),
        iterators::end(rng));
      return std::forward<Rng>(rng);
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const {
      GENEX_ASSERT(std::out_of_range, n >= 0);
      return meta::bind_back(drop_last_fn{}, n);
    }
  };

  export inline constexpr drop_last_fn drop_last{};
}