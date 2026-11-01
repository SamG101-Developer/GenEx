module;
#include <genex/detail/macros.hpp>

export module genex.views.drop_while;
export import genex.pipe;
import genex.concepts;
import genex.detail.predicate_iters;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept droppable_while_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj> and
    std::move_constructible<Pred> and
    std::move_constructible<Proj>;

  template <typename Rng, typename Pred, typename Proj>
  concept droppable_while_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    droppable_while_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::views::detail::impl {
  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::droppable_while_iters<I, S, Pred, Proj>
  struct drop_while_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Pred pred;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr drop_while_view(
      I first, S last, Pred pred, Proj proj) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      pred(std::move(pred)), proj(std::move(proj)) {
    }

    [[nodiscard]] constexpr auto begin() const -> I {
      auto first = it;
      auto keep_dropping = pred;
      auto project = proj;
      while (first != st and meta::invoke(
        keep_dropping, meta::invoke(project, *first))) { ++first; }
      return first;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const -> S {
      return st;
    }

    [[nodiscard]] constexpr auto size() const
      requires std::sized_sentinel_for<S, I> {
      return st - begin();
    }
  };
}

namespace genex::views {
  struct drop_while_fn : pipe_closure {
    template <typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::droppable_while_iters<I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(drop_while_view, I, S, Pred, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      return detail::impl::drop_while_view(
        std::move(first), std::move(last), std::move(pred), std::move(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::droppable_while_range<Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(drop_while_view, iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj) and
      SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::drop_while_view(
        std::move(first), std::move(last), std::move(pred), std::move(proj));
    }

    template <typename Pred, typename Proj = meta::identity>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Pred pred, Proj proj = {}) const noexcept(
      SAFE_CTOR(drop_while_fn) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      return meta::bind_back(drop_while_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr drop_while_fn drop_while{};
}
