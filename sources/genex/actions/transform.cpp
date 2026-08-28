module;
#include <genex/macros.hpp>

export module genex.actions.transform;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename F, typename Proj>
  concept transformable_range =
    input_range<Rng> and
    std::indirectly_unary_invocable<Proj, iterator_t<Rng>> and
    std::invocable<F&, std::invoke_result_t<Proj&, range_reference_t<Rng>>> and
    std::assignable_from<
      range_reference_t<Rng>,
      std::invoke_result_t<F&, std::invoke_result_t<Proj&, range_reference_t<Rng>>>>;
}

namespace genex::actions {
  struct transform_fn {
    template <typename Rng, typename F, typename Proj = meta::identity>
      requires detail::concepts::transformable_range<Rng, F, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, F f, Proj proj = {}) const -> decltype(auto) {
      auto [first, last] = iterators::iter_pair(rng);
      for (; first != last; ++first) {
        *first = meta::invoke(f, meta::invoke(proj, *first));
      }
      return std::forward<Rng>(rng);
    }

    template <typename F, typename Proj = meta::identity>
      requires (not range<F>)
    GENEX_INLINE constexpr auto operator()(F f, Proj proj = {}) const {
      return meta::bind_back(transform_fn{}, std::move(f), std::move(proj));
    }
  };

  export inline constexpr transform_fn transform{};
}