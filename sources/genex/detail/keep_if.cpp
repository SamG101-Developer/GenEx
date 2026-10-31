export module genex.detail.keep_if;
import genex.concepts;
import genex.detail.partition_impl;
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
    while (first != last) {
      if (keep(*first)) { ++first; }
      else {
        first = rng.erase(first);
        last = iterators::end(rng);
      }
    }
  }

  export template <typename Rng, typename Keep>
  requires permutable<iterator_t<Rng>>
  constexpr auto keep_if(Rng &rng, Keep keep) -> void {
    auto first = iterators::begin(rng);
    auto last = iterators::end(rng);
    while (first != last and keep(*first)) { ++first; }
    if (first == last) { return; }

    auto dest = first;
    ++first;
    if constexpr (algorithms::detail::cheap_elements<iterator_t<Rng>>) {
      // A branch is wrong half the time when "keep" can't be guessed, and always
      // writing is slower when it can, so which to use is decided by the data:
      // a sample that often flips between keeping and removing means the branch
      // will be mispredicted enough to cost more than the writes do. What's at
      // "dest" has already been moved along or removed, so it can be overwritten.
      using diff_t = iter_difference_t<iterator_t<Rng>>;
      constexpr auto sample = diff_t{64};
      constexpr auto stretch = diff_t{1024};
      constexpr auto hard_flips = 8;
      while (first != last) {
        const auto sampled = std::min(last - first, sample);
        auto mask = std::uint64_t{0};
        for (auto i = diff_t{0}; i < sampled; ++i) {
          mask |= static_cast<std::uint64_t>(static_cast<bool>(keep(first[i]))) << i;
        }
        const auto pairs = sampled > 1 ? ~std::uint64_t{0} >> (65 - sampled) : std::uint64_t{0};
        const auto hard = std::popcount((mask ^ (mask >> 1)) & pairs) >= hard_flips;
        for (; mask != 0; mask &= mask - 1) {
          *dest = first[static_cast<diff_t>(std::countr_zero(mask))];
          ++dest;
        }
        first += sampled;

        const auto stop = first + std::min(last - first, stretch);
        if (hard) {
          for (; first != stop; ++first) {
            const bool kept = keep(*first);
            *dest = *first;
            dest += kept;
          }
        }
        else {
          for (; first != stop; ++first) {
            if (keep(*first)) {
              *dest = *first;
              ++dest;
            }
          }
        }
      }
    }
    else {
      for (; first != last; ++first) {
        if (keep(*first)) {
          *dest = std::move(*first);
          ++dest;
        }
      }
    }
    rng.erase(std::move(dest), std::move(last));
  }
}
