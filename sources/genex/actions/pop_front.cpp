module;
#include <genex/detail/macros.hpp>

export module genex.actions.pop_front;
export import genex.pipe;
import genex.actions.erase;
import genex.concepts;
import genex.iterators.access;

namespace genex::actions::detail::concepts {
  template <typename Rng>
  concept front_poppable_range =
    range<Rng>;

  template <typename Rng>
  concept front_poppable_select_pop_front =
    front_poppable_range<Rng> and
    has_member_pop_front<Rng>;

  template <typename Rng>
  concept front_poppable_select_erase =
    front_poppable_range<Rng> and
    has_member_erase<Rng> and
    not front_poppable_select_pop_front<Rng>;
}

namespace genex::actions {
  struct pop_front_fn : pipe_closure {
    template <typename Rng>
    requires detail::concepts::front_poppable_select_pop_front<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
      return rng.pop_front();
    }

    template <typename Rng>
    requires detail::concepts::front_poppable_select_erase<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
      return actions::erase(rng, iterators::begin(rng));
    }
  };

  export inline constexpr pop_front_fn pop_front{};
}
