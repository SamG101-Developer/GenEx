module;
#include <genex/macros.hpp>

export module genex.actions.take_last;
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
  concept takeable_last_range =
    forward_range<Rng> and
    has_member_erase<Rng> and
    std::weakly_incrementable<Int>;
}

namespace genex::actions {
  struct take_last_fn {
    /**
     * Keeps the final @c n elements, erasing everything before them. Asking for more elements than the
     * range holds keeps the whole range.
     */
    template <typename Rng, typename Int>
      requires detail::concepts::takeable_last_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const -> decltype(auto) {
      GENEX_ASSERT(std::out_of_range, n >= 0);
      const auto len = iterators::distance(iterators::begin(rng), iterators::end(rng));
      const auto count = std::min(static_cast<std::ptrdiff_t>(n), len);
      actions::erase(
        rng, iterators::begin(rng),
        iterators::next(iterators::begin(rng), static_cast<range_difference_t<Rng>>(len - count)));
      return std::forward<Rng>(rng);
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const {
      GENEX_ASSERT(std::out_of_range, n >= 0);
      return meta::bind_back(take_last_fn{}, n);
    }
  };

  export inline constexpr take_last_fn take_last{};
}