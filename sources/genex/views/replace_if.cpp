module;
#include <genex/detail/macros.hpp>

export module genex.views.replace_if;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.detail.predicate_iters;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Pred, typename New, typename Proj>
  concept replaceable_if_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj> and
    std::convertible_to<New, iter_value_t<I>> and
    std::move_constructible<Pred> and
    std::move_constructible<Proj>;

  template <typename Rng, typename Pred, typename New, typename Proj>
  concept replaceable_if_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    replaceable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, New, Proj>;
}

namespace genex::views::detail::impl {
  template <typename I, typename S, typename Pred, typename New, typename Proj>
  requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
  struct replace_iterator :
    genex::detail::adaptor_ops<replace_iterator<I, S, Pred, New, Proj>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Pred pred;
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
      I first, S last, Pred pred, New new_val, Proj proj) :
      it(std::move(first)), st(std::move(last)),
      pred(std::move(pred)), new_value(std::move(new_val)),
      proj(std::move(proj)) {
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return meta::invoke(pred, meta::invoke(proj, *it)) ? new_value : *it;
    }
  };

  template <typename I, typename S, typename Pred, typename New, typename Proj>
  requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
  struct replace_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Pred pred;
    GENEX_NO_UNIQUE_ADDRESS New new_value;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr replace_view(
      I first, S last, Pred pred, New new_val, Proj proj) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and
      SAFE_MOVE(New) and SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      pred(std::move(pred)), new_value(std::move(new_val)), proj(std::move(proj)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return replace_iterator(it, st, pred, new_value, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return replace_iterator(st, st, pred, new_value, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct replace_if_fn : pipe_closure {
    template <
      typename I, typename S, typename Pred, typename New,
      typename Proj = meta::identity>
    requires detail::concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, New new_value, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(replace_view, I, S, Pred, New, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and
      SAFE_MOVE(New) and SAFE_MOVE(Proj)) {
      return detail::impl::replace_view(
        std::move(first), std::move(last), std::move(pred),
        std::move(new_value), std::move(proj));
    }

    template <
      typename Rng, typename Pred, typename New, typename Proj = meta::identity>
    requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, New new_value, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(replace_view, iterator_t<Rng>, sentinel_t<Rng>, Pred, New, Proj) and
      SAFE_MOVE(Pred) and SAFE_MOVE(New) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::replace_view(
        std::move(first), std::move(last), std::move(pred),
        std::move(new_value), std::move(proj));
    }

    template <typename Pred, typename New, typename Proj = meta::identity>
    requires closure_callable<Pred>
    GENEX_INLINE constexpr auto operator()(
      Pred pred, New new_value, Proj proj = {}) const noexcept(
      SAFE_CTOR(replace_if_fn) and SAFE_MOVE(Pred) and
      SAFE_MOVE(New) and SAFE_MOVE(Proj)) {
      return meta::bind_back(
        replace_if_fn{}, std::move(pred),
        std::move(new_value), std::move(proj));
    }
  };

  export inline constexpr replace_if_fn replace_if{};
}
