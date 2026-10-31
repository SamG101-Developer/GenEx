module;
#include <genex/detail/macros.hpp>

export module genex.views.replace;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Old, typename New, typename Proj>
  concept replaceable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_equivalence_relation<
      operations::eq, std::projected<I, Proj>, std::remove_cvref_t<Old> const*> and
    std::convertible_to<New, iter_value_t<I>> and
    std::move_constructible<Proj>;

  template <typename Rng, typename Old, typename New, typename Proj>
  concept replaceable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    replaceable_iters<iterator_t<Rng>, sentinel_t<Rng>, Old, New, Proj>;
}

namespace genex::views::detail::impl {
  template <typename I, typename S, typename Old, typename New, typename Proj>
  requires concepts::replaceable_iters<I, S, Old, New, Proj>
  struct replace_iterator :
    genex::detail::adaptor_ops<replace_iterator<I, S, Old, New, Proj>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Old old_value;
    GENEX_NO_UNIQUE_ADDRESS New new_value;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_concept = iter_strength_t<I>;
    using iterator_category = legacy_iter_category_t<
      reference, iterator_concept>;

    GENEX_INLINE constexpr replace_iterator() = default;

    GENEX_INLINE constexpr replace_iterator(
      I first, S last, Old old_val, New new_val, Proj proj) :
      it(std::move(first)), st(std::move(last)),
      old_value(std::move(old_val)), new_value(std::move(new_val)),
      proj(std::move(proj)) {
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return operations::eq{}(
        meta::invoke(proj, *it), old_value) ? new_value : *it;
    }
  };

  template <typename I, typename S, typename Old, typename New, typename Proj>
  requires concepts::replaceable_iters<I, S, Old, New, Proj>
  struct replace_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Old old_value;
    GENEX_NO_UNIQUE_ADDRESS New new_value;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr replace_view(
      I first, S last, Old old_val, New new_val, Proj proj) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Old) and SAFE_MOVE(New) and
      SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      old_value(std::move(old_val)), new_value(std::move(new_val)),
      proj(std::move(proj)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return replace_iterator(it, st, old_value, new_value, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return replace_iterator(st, st, old_value, new_value, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct replace_fn : pipe_closure {
    template <
      typename I, typename S, typename Old, typename New,
      typename Proj = meta::identity>
    requires detail::concepts::replaceable_iters<I, S, Old, New, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Old old_value, New new_value,
      Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(replace_view, I, S, Old, New, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Old) and SAFE_MOVE(New) and SAFE_MOVE(Proj)) {
      return detail::impl::replace_view(
        std::move(first), std::move(last),
        std::move(old_value), std::move(new_value), std::move(proj));
    }

    template <
      typename Rng, typename Old, typename New,
      typename Proj = meta::identity>
    requires detail::concepts::replaceable_range<Rng, Old, New, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Old old_value, New new_value, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(replace_view, iterator_t<Rng>, sentinel_t<Rng>, Old, New, Proj) and
      SAFE_MOVE(Old) and SAFE_MOVE(New) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::replace_view(
        std::move(first), std::move(last),
        std::move(old_value), std::move(new_value), std::move(proj));
    }

    template <typename Old, typename New, typename Proj = meta::identity>
    requires closure_value<Old>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Old &&old_value, New new_value, Proj proj = {}) const noexcept(
      SAFE_CTOR(replace_fn) and SAFE_CTOR(std::decay_t<Old>, Old) and
      SAFE_MOVE(New) and SAFE_MOVE(Proj)) {
      return meta::bind_back(
        replace_fn{}, std::forward<Old>(old_value), std::move(new_value), std::move(proj));
    }
  };

  export inline constexpr replace_fn replace{};
}
