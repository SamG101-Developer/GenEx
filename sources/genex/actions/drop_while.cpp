module;
#include <genex/detail/macros.hpp>

export module genex.actions.drop_while;
export import genex.pipe;
import genex.actions.erase;
import genex.algorithms.find_if_not;
import genex.concepts;
import genex.iterators.access;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Pred>
  concept droppable_while_range =
    forward_range<Rng> and
    indirect_unary_predicate<Pred, iterator_t<Rng>>;
}

namespace genex::actions {
  struct drop_while_fn : pipe_closure {
    template <typename Rng, typename Pred>
    requires detail::concepts::droppable_while_range<Rng, Pred>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred) const -> Rng {
      actions::erase(
        rng, iterators::begin(rng),
        genex::find_if_not(
          iterators::begin(rng), iterators::end(rng), std::move(pred)));
      return std::forward<Rng>(rng);
    }

    template <typename Pred>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Pred pred) const {
      return meta::bind_back(drop_while_fn{}, std::move(pred));
    }
  };

  export inline constexpr drop_while_fn drop_while{};
}
