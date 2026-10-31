module;
#include <genex/detail/macros.hpp>

export module genex.views.cycle;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept cyclable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I>;

  template <typename Rng>
  concept cyclable_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    cyclable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  struct cycle_sentinel {};

  template <typename I, typename S>
  requires concepts::cyclable_iters<I, S>
  struct cycle_iterator : genex::detail::iterator_ops<cycle_iterator<I, S>> {
    I it;
    I begin;
    S st;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::conditional_t<
      std::bidirectional_iterator<I>,
      std::bidirectional_iterator_tag,
      std::forward_iterator_tag>;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr cycle_iterator() = default;
    GENEX_INLINE constexpr cycle_iterator(I first, I begin, S last) :
      it(std::move(first)), begin(std::move(begin)), st(std::move(last)) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      if (it == st) { it = begin; }
    }

    GENEX_INLINE constexpr auto prev() -> void
    requires std::bidirectional_iterator<decltype(it)> {
      if (it == begin) { it = st; }
      --it;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const cycle_iterator &self, const cycle_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const cycle_iterator &, cycle_sentinel) -> bool {
      return false;
    }
  };

  template <typename I, typename S>
  requires concepts::cyclable_iters<I, S>
  struct cycle_view : view_base {
    I it;
    I it_cpy;
    S st;

    GENEX_INLINE constexpr cycle_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_CTOR(I, I&)) :
      it(std::move(first)), it_cpy(it), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return cycle_iterator<I, S>(it, it_cpy, st);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return cycle_sentinel();
    }
  };
}

namespace genex::views {
  struct cycle_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::cyclable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_IMPL_CTOR(cycle_view, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::cycle_view(std::move(first), std::move(last));
    }

    template <typename Rng>
    requires detail::concepts::cyclable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(cycle_view, iterator_t<Rng>, sentinel_t<Rng>) and
      SAFE_MOVE(iterator_t<Rng>) and SAFE_MOVE(sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::cycle_view(std::move(first), std::move(last));
    }
  };

  export inline constexpr cycle_fn cycle{};
}
