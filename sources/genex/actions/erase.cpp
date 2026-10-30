module;
#include <genex/detail/macros.hpp>

export module genex.actions.erase;
export import genex.pipe;
import genex.concepts;
import genex.iterators.access;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename I, typename S>
  concept erasable_range =
    range<Rng> and
    std::sentinel_for<S, I> and
    std::same_as<std::remove_cvref_t<I>, iterator_t<Rng>> and
    std::same_as<std::remove_cvref_t<S>, sentinel_t<Rng>> and
    has_member_erase<Rng>;
}

namespace genex::actions {
  struct erase_fn final {
    template <typename Rng, typename I, typename S>
      requires detail::concepts::erasable_range<Rng, I, S>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, I it, S end) const -> decltype(auto) {
      return rng.erase(std::move(it), std::move(end));
    }

    template <typename Rng, typename I>
      requires detail::concepts::erasable_range<Rng, I, sentinel_t<Rng>>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, I it) const {
      return rng.erase(std::move(it));
    }
  };

  export inline constexpr erase_fn erase{};
}

namespace genex::actions::detail {
  /**
   * Keeps the elements @c keep says to, in order, in a single pass: each is moved straight to its final place, and
   * everything left over is erased at once. Erasing one at a time would shift the whole tail for every element removed.
   * Elements that can't be assigned to, such as a @c std::set's, have no choice but to be erased individually.
   */
  export template <typename Rng, typename Keep>
  constexpr auto keep_if(Rng &rng, Keep keep) -> void {
    auto first = iterators::begin(rng);
    auto last = iterators::end(rng);
    if constexpr (std::permutable<iterator_t<Rng>>) {
      while (first != last and keep(*first)) { ++first; }
      if (first == last) { return; }

      auto dest = first;
      for (++first; first != last; ++first) {
        if (keep(*first)) {
          *dest = std::move(*first);
          ++dest;
        }
      }
      rng.erase(std::move(dest), std::move(last));
    }
    else {
      while (first != last) {
        if (keep(*first)) { ++first; }
        else {
          first = rng.erase(first);
          last = iterators::end(rng);
        }
      }
    }
  }
}
