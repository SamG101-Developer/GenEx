module;
#include <genex/macros.hpp>

export module genex.actions.rotate;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.access;
import genex.iterators.next;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Int>
  concept rotatable_range =
    forward_range<Rng> and
    std::permutable<iterator_t<Rng>> and
    std::same_as<iterator_t<Rng>, sentinel_t<Rng>> and
    std::weakly_incrementable<Int>;
}

namespace genex::actions {
  struct rotate_fn {
    template <typename Rng, typename Int>
      requires detail::concepts::rotatable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const -> decltype(auto) {
      GENEX_ASSERT(std::out_of_range, n >= 0);
      auto first = iterators::begin(rng);
      auto last = iterators::end(rng);
      auto mid = iterators::next(first, static_cast<range_difference_t<Rng>>(n), last);
      std::rotate(std::move(first), std::move(mid), std::move(last));
      return std::forward<Rng>(rng);
    }

    template <typename Int>
      requires std::weakly_incrementable<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const {
      GENEX_ASSERT(std::out_of_range, n >= 0);
      return meta::bind_back(rotate_fn{}, n);
    }
  };

  export inline constexpr rotate_fn rotate{};
}