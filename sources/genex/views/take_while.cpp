module;
#include <genex/detail/macros.hpp>

export module genex.views.take_while;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Pred, typename Proj>
  concept takeable_while_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

  template <typename Rng, typename Pred, typename Proj>
  concept takeable_while_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    takeable_while_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}

namespace genex::views::detail::impl {
  struct take_while_sentinel {};

  template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::takeable_while_iters<I, S, Pred, Proj>
  struct take_while_iterator : genex::detail::iterator_ops<take_while_iterator<I, S, Pred, Proj>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS mutable meta::box<Pred> pred;
    GENEX_NO_UNIQUE_ADDRESS mutable meta::box<Proj> proj;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr take_while_iterator() = default;

    GENEX_INLINE constexpr take_while_iterator(I it, S st, Pred pred, Proj proj) :
      it(std::move(it)), st(std::move(st)),
      pred(std::move(pred)), proj(std::move(proj)) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
    }

    GENEX_INLINE constexpr auto prev() -> void requires std::bidirectional_iterator<decltype(it)> {
      --it;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const take_while_iterator &self, const take_while_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(const take_while_iterator &self, take_while_sentinel) -> bool {
      return self.it == self.st or not meta::invoke(*self.pred, meta::invoke(*self.proj, *self.it));
    }
  };

  template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::takeable_while_iters<I, S, Pred, Proj>
  struct take_while_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Pred pred;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr take_while_view(I first, S last, Pred pred, Proj proj) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      pred(std::move(pred)), proj(std::move(proj)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return take_while_iterator(it, st, pred, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return take_while_sentinel();
    }

    // Finding where to stop first means the rest can be copied in one go, with each element only being tested once.
    template <typename Out>
      requires std::derived_from<iter_category_t<I>, std::forward_iterator_tag>
      and requires(Out &out, I i) { out.insert(out.end(), i, i); }
    constexpr auto append_to(Out &out) const -> void {
      auto stop = it;
      auto keep_taking = pred;
      auto project = proj;
      while (stop != st and meta::invoke(keep_taking, meta::invoke(project, *stop))) { ++stop; }
      out.insert(out.end(), it, stop);
    }
  };
}

namespace genex::views {
  struct take_while_fn {
    template <typename I, typename S, typename Pred, typename Proj = meta::identity>
      requires detail::concepts::takeable_while_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(take_while_view, I, S, Pred, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      return detail::impl::take_while_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
    }

    template <typename Rng, typename Pred, typename Proj = meta::identity>
      requires detail::concepts::takeable_while_range<Rng, Pred, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(take_while_view, iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj) and
      SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::take_while_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
    }

    template <typename Pred, typename Proj = meta::identity>
      requires (not range<Pred>)
    GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const noexcept(
      SAFE_CTOR(take_while_fn) and SAFE_MOVE(Pred) and SAFE_MOVE(Proj)) {
      return meta::bind_back(take_while_fn{}, std::move(pred), std::move(proj));
    }
  };

  export inline constexpr take_while_fn take_while{};
}
