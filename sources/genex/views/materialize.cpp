module;
#include <genex/detail/macros.hpp>

export module genex.views.materialize;
export import genex.pipe;
import genex.actions.push_back;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.subrange;
import genex.to_container;
import std;

namespace genex::views::detail::concepts {
  template <template <typename> typename Cache, typename I, typename S>
  concept materializable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    actions::detail::concepts::back_insertable_range<Cache<iter_value_t<I>>, iter_value_t<I>> and
    std::is_constructible_v<Cache<iter_value_t<I>>>;

  template <template <typename> typename Cache, typename Rng>
  concept materializable_range =
    input_range<Rng> and
    materializable_iters<Cache, iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  template <template <typename> typename Cache, typename I, typename S>
    requires concepts::materializable_iters<Cache, I, S>
  GENEX_INLINE constexpr auto do_materialize(I first, S last) -> Cache<iter_value_t<I>> {
    return genex::to_base_fn<Cache>(genex::subrange(std::move(first), std::move(last)));
  }
}

namespace genex::views {
  template <template <typename> typename Cache>
  struct materialize_fn {
    template <typename I, typename S>
      requires detail::concepts::materializable_iters<Cache, I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const {
      return detail::impl::do_materialize<Cache>(std::move(first), std::move(last));
    }

    template <typename Rng>
      requires detail::concepts::materializable_range<Cache, Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::do_materialize<Cache>(std::move(first), std::move(last));
    }

    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(materialize_fn)) {
      return meta::bind_back(materialize_fn{});
    }
  };

  export inline constexpr materialize_fn<std::vector> materialize{};
}
