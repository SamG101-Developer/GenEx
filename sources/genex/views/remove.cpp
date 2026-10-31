module;
#include <genex/detail/macros.hpp>

export module genex.views.remove;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename E, typename Proj>
  concept removable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_equivalence_relation<
      operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*> and
    std::move_constructible<Proj>;

  template <typename Rng, typename E, typename Proj>
  concept removable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    removable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}

namespace genex::views::detail::impl {
  struct remove_sentinel {};

  template <typename I, typename S, typename E, typename Proj>
  requires concepts::removable_iters<I, S, E, Proj>
  struct remove_iterator :
    genex::detail::iterator_ops<remove_iterator<I, S, E, Proj>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS E value;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr remove_iterator() = default;

    GENEX_INLINE constexpr remove_iterator(I first, S last, E val, Proj proj) :
      it(std::move(first)), st(std::move(last)),
      value(std::move(val)), proj(std::move(proj)) {
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const remove_iterator &self, const remove_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const remove_iterator &self, remove_sentinel) -> bool {
      return self.it == self.st;
    }

  private:
    constexpr auto fwd_to_valid() -> void {
      while (it != st and operations::eq{}(std::invoke(proj, *it), value)) { ++it; }
    }
  };

  template <typename I, typename S, typename E, typename Proj>
  requires concepts::removable_iters<I, S, E, Proj>
  struct remove_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS E value;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr remove_view(
      I first, S last, E val, Proj proj) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(E) and SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      value(std::move(val)), proj(std::move(proj)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return remove_iterator{it, st, value, proj};
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return remove_sentinel();
    }
  };
}

namespace genex::views {
  struct remove_fn : pipe_closure {
    template <
      typename I, typename S, typename E, typename Proj = meta::identity>
    requires detail::concepts::removable_iters<I, S, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, E value, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(remove_view, I, S, E, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(E) and SAFE_MOVE(Proj)) {
      return detail::impl::remove_view(
        std::move(first), std::move(last), std::move(value), std::move(proj));
    }

    template <typename Rng, typename E, typename Proj = meta::identity>
    requires detail::concepts::removable_range<Rng, E, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, E value, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(remove_view, iterator_t<Rng>, sentinel_t<Rng>, E, Proj) and
      SAFE_MOVE(E) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::remove_view(
        std::move(first), std::move(last), std::move(value), std::move(proj));
    }

    template <typename E, typename Proj = meta::identity>
    requires closure_value<E>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      E &&value, Proj proj = {}) const noexcept(
      SAFE_CTOR(remove_fn) and SAFE_CTOR(std::decay_t<E>, E) and SAFE_MOVE(Proj)) {
      return meta::bind_back(remove_fn{}, std::forward<E>(value), std::move(proj));
    }
  };

  export inline constexpr remove_fn remove{};
}
