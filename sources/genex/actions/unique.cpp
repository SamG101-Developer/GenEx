module;
#include <genex/detail/macros.hpp>

export module genex.actions.unique;
export import genex.pipe;
import genex.actions.erase;
import genex.concepts;
import genex.iterators.access;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Comp, typename Proj>
  concept uniqueable_range =
    forward_range<Rng> and
    has_member_erase<Rng> and
    std::indirectly_comparable<iterator_t<Rng>, iterator_t<Rng>, Comp, Proj, Proj>;
}

namespace genex::actions {
  struct unique_fn : pipe_closure {
    template <
      typename Rng, typename Comp = operations::eq,
      typename Proj = meta::identity>
    requires detail::concepts::uniqueable_range<Rng, Comp, Proj>
    constexpr auto operator()(
      Rng &&rng, Comp comp = {}, Proj proj = {}) const -> Rng {
      auto first = iterators::begin(rng);
      auto last = iterators::end(rng);
      if (first == last) { return std::forward<Rng>(rng); }

      if constexpr (std::permutable<iterator_t<Rng>>) {
        auto kept = first;
        for (++first; first != last; ++first) {
          if (meta::invoke(comp, meta::invoke(proj, *kept), meta::invoke(proj, *first))) { break; }
          kept = first;
        }
        if (first == last) { return std::forward<Rng>(rng); }

        for (++first; first != last; ++first) {
          if (not meta::invoke(comp, meta::invoke(proj, *kept), meta::invoke(proj, *first))) {
            *++kept = std::move(*first);
          }
        }
        rng.erase(++kept, std::move(last));
      }
      else {
        auto kept = first;
        for (++first; first != last;) {
          if (meta::invoke(comp, meta::invoke(proj, *kept), meta::invoke(proj, *first))) {
            first = actions::erase(rng, first);
            last = iterators::end(rng);
          }
          else {
            kept = first;
            ++first;
          }
        }
      }
      return std::forward<Rng>(rng);
    }

    template <typename Comp = operations::eq, typename Proj = meta::identity>
    requires (not range<Comp>)
    GENEX_INLINE constexpr auto operator()(Comp comp, Proj proj = {}) const {
      return meta::bind_back(unique_fn{}, std::move(comp), std::move(proj));
    }
  };

  export inline constexpr unique_fn unique{};
}