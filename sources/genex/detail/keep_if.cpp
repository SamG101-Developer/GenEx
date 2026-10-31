export module genex.detail.keep_if;
import genex.concepts;
import genex.iterators.access;
import std;

namespace genex::actions::detail {
  /// Keeps the elements "keep(...)" says to, in order, in a single pass: each
  /// is moved straight to its final place, and everything left over is erased
  /// at once. Erasing one at a time would shift the whole tail for every
  /// element removed. Elements that can't be assigned to, such as a std::set's,
  /// have no choice but to be erased individually.
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
