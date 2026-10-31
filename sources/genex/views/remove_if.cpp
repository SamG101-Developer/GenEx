module;
#include <genex/detail/macros.hpp>

export module genex.views.remove_if;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.detail.predicate_iters;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <
    typename I, typename S, typename Pred, typename Proj = meta::identity>
  concept removable_if_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj> and
    std::move_constructible<Pred> and
    std::move_constructible<Proj>;

  template <typename Rng, typename Pred, typename Proj = meta::identity>
  concept removable_if_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    removable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::views::detail::impl {
  struct remove_if_sentinel {};

  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::removable_if_iters<I, S, Pred, Proj>
  struct remove_if_iterator : genex::detail::iterator_ops<remove_if_iterator<I, S, Pred, Proj>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS meta::box<Pred> pred;
    GENEX_NO_UNIQUE_ADDRESS meta::box<Proj> proj;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr remove_if_iterator() = default;

    GENEX_INLINE constexpr remove_if_iterator(
      I first, S last, Pred p, Proj proj) :
      it(std::move(first)), st(std::move(last)),
      pred(std::move(p)), proj(std::move(proj)) {
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
      const remove_if_iterator &self, const remove_if_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const remove_if_iterator &self, remove_if_sentinel) -> bool {
      return self.it == self.st;
    }

  private:
    constexpr auto fwd_to_valid() -> void {
      while (it != st and meta::invoke(*pred, meta::invoke(*proj, *it))) {
        ++it;
      }
    }
  };

  template <typename I, typename S, typename Pred, typename Proj>
  requires concepts::removable_if_iters<I, S, Pred, Proj>
  struct remove_if_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Pred pred;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr remove_if_view(
      I first, S last, Pred p, Proj prj = {}) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      pred(std::move(p)), proj(std::move(prj)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return remove_if_iterator(it, st, pred, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return remove_if_sentinel();
    }
  };
}

namespace genex::views {
  struct remove_if_fn : pipe_closure {
    template <
      typename I, typename S, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::removable_if_iters<I, S, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Pred pred, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(remove_if_view, I, S, Pred, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      return detail::impl::remove_if_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
    }

    template <
      typename Rng, typename Pred, typename Proj = meta::identity>
    requires detail::concepts::removable_if_range<Rng, Pred, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Pred pred, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(remove_if_view, iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj) and
      SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::remove_if_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
    }

    template <typename Pred, typename Proj = meta::identity>
    requires closure_callable<Pred>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Pred pred, Proj proj = {}) const noexcept(
      SAFE_CTOR(remove_if_fn) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      return meta::bind_back(remove_if_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr remove_if_fn remove_if{};
}
