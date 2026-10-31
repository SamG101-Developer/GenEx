module;
#include <genex/detail/macros.hpp>

export module genex.views.unique;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Comp, typename Proj>
  concept uniqueable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirectly_comparable<I, I, Comp, Proj, Proj> and
    std::move_constructible<Comp> and
    std::move_constructible<Proj>;

  template <typename Rng, typename Comp, typename Proj>
  concept uniqueable_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    uniqueable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}

namespace genex::views::detail::impl {
  struct unique_sentinel {};

  template <typename I, typename S, typename Comp, typename Proj>
  requires concepts::uniqueable_iters<I, S, Comp, Proj>
  struct unique_iterator :
    genex::detail::iterator_ops<unique_iterator<I, S, Comp, Proj>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS meta::box<Comp> comp;
    GENEX_NO_UNIQUE_ADDRESS meta::box<Proj> proj;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr unique_iterator() = default;

    GENEX_INLINE constexpr unique_iterator(
      I first, S last, Comp comp, Proj proj) :
      it(std::move(first)), st(std::move(last)),
      comp(std::move(comp)), proj(std::move(proj)) {
    }

    /// Every element skipped here compares equal to the one just yielded, so
    /// comparing each candidate against that single element is enough for an
    /// equivalence relation (no need to re-anchor).
    GENEX_INLINE constexpr auto next() -> void {
      auto prev = it;
      ++it;
      while (it != st and meta::invoke(
        *comp, meta::invoke(*proj, *it), meta::invoke(*proj, *prev))) { ++it; }
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const unique_iterator &self, const unique_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const unique_iterator &self, unique_sentinel) -> bool {
      return self.it == self.st;
    }
  };

  template <typename I, typename S, typename Comp, typename Proj>
  requires concepts::uniqueable_iters<I, S, Comp, Proj>
  struct unique_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Comp comp;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr unique_view(
      I first, S last, Comp comp, Proj proj = {}) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      comp(std::move(comp)), proj(std::move(proj)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return unique_iterator<I, S, Comp, Proj>(it, st, comp, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return unique_sentinel();
    }
  };
}

namespace genex::views {
  struct unique_fn : pipe_closure {
    template <
      typename I, typename S, typename Comp = operations::eq,
      typename Proj = meta::identity>
    requires detail::concepts::uniqueable_iters<I, S, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Comp comp = {}, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(unique_view, I, S, Comp, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      return detail::impl::unique_view(
        std::move(first), std::move(last), std::move(comp), std::move(proj));
    }

    template <
      typename Rng, typename Comp = operations::eq,
      typename Proj = meta::identity>
    requires detail::concepts::uniqueable_range<Rng, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Comp comp = {}, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(unique_view, iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj) and
      SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::unique_view(
        std::move(first), std::move(last), std::move(comp), std::move(proj));
    }

    template <typename Comp = operations::eq, typename Proj = meta::identity>
    requires closure_callable<Comp>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Comp comp, Proj proj = {}) const noexcept(
      SAFE_CTOR(unique_fn) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      return meta::bind_back(unique_fn{}, std::move(comp), std::move(proj));
    }
  };

  export inline constexpr unique_fn unique{};
}