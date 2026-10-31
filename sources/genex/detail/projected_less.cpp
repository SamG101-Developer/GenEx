module;
#include <genex/detail/macros.hpp>

export module genex.detail.projected_less;
import genex.meta;
import std;

export namespace genex::detail {
  /// A comparison with a projection applied to both sides, which is the form
  /// that all the sorting code uses. It refers to them rather than holding them,
  /// so stateful ones are only ever copied once, by the algorithm's parameters.
  template <typename Comp, typename Proj>
  struct projected_less {
    Comp &comp;
    Proj &proj;

    template <typename A, typename B>
    GENEX_INLINE constexpr auto operator()(A &&a, B &&b) const -> bool {
      return meta::invoke(
        comp,
        meta::invoke(proj, std::forward<A>(a)),
        meta::invoke(proj, std::forward<B>(b)));
    }
  };
}
