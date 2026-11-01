module;
#include <genex/detail/macros.hpp>

export module genex.actions.replace_if;
export import genex.pipe;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Pred, typename New, typename Proj>
  concept replaceable_if_range =
    forward_range<Rng> and
    permutable<iterator_t<Rng>> and
    indirect_unary_predicate<Pred, projected<iterator_t<Rng>, Proj>> and
    std::convertible_to<New, range_value_t<Rng>>;
}

namespace genex::actions {
  struct replace_if_fn : pipe_closure {
    template <
      typename Rng, typename Pred, typename New, typename Proj = meta::identity>
    requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj>
    constexpr auto operator()(
      Rng &&rng, Pred pred, New new_val, Proj proj = {}) const -> Rng {
      auto [first, last] = iterators::iter_pair(rng);
      for (; first != last; ++first) {
        if (meta::invoke(pred, meta::invoke(proj, *first))) {
          *first = new_val;
        }
      }
      return std::forward<Rng>(rng);
    }

    template <typename Pred, typename New, typename Proj = meta::identity>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Pred pred, New new_val, Proj proj = {}) const noexcept(
      SAFE_CTOR(replace_if_fn) and SAFE_MOVE(Pred) and SAFE_MOVE(New) and
      SAFE_MOVE(Proj)) {
      return meta::bind_back(
        replace_if_fn{}, std::move(pred), std::move(new_val), std::move(proj));
    }
  };

  export inline constexpr replace_if_fn replace_if{};
}
