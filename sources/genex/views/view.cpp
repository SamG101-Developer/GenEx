module;
#include <genex/detail/macros.hpp>

export module genex.views.view;
export import genex.pipe;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.span;
import genex.subrange;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept viewable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I>;

  template <typename Rng>
  concept viewable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    viewable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views {
  struct view_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::viewable_iters<I, S>
      and std::contiguous_iterator<I>
      and std::sized_sentinel_for<S, I>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_CTOR(genex::span<iter_element_t<I>>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return genex::span<iter_element_t<I>>(
        std::move(first), std::move(last));
    }

    template <typename I, typename S>
    requires detail::concepts::viewable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last) const noexcept(
      SAFE_CTOR(genex::subrange<I, S>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return genex::subrange<I, S>(
        std::move(first), std::move(last));
    }

    template <typename Rng>
    requires detail::concepts::viewable_range<Rng>
      and std::contiguous_iterator<iterator_t<Rng>>
      and std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CTOR(genex::span<range_element_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return genex::span<range_element_t<Rng>>(
        std::move(first), std::move(last));
    }

    template <typename Rng>
    requires detail::concepts::viewable_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CTOR(genex::subrange<iterator_t<Rng>, sentinel_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return genex::subrange<iterator_t<Rng>, sentinel_t<Rng>>(
        std::move(first), std::move(last));
    }
  };

  export inline constexpr view_fn view{};
}
