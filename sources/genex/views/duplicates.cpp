module;
#include <genex/detail/macros.hpp>

export module genex.views.duplicates;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Comp, typename Proj>
  concept duplicate_checkable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    indirectly_comparable<I, I, Comp, Proj, Proj> and
    std::move_constructible<Comp> and
    std::move_constructible<Proj>;

  template <typename Rng, typename Comp, typename Proj>
  concept duplicate_checkable_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    duplicate_checkable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}

namespace genex::views::detail::impl {
  struct duplicate_sentinel {};

  template <typename I, typename S, typename Comp, typename Proj>
  requires concepts::duplicate_checkable_iters<I, S, Comp, Proj>
  struct duplicate_iterator :
    genex::detail::iterator_ops<duplicate_iterator<I, S, Comp, Proj>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Comp comp;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    // Duplicate state attributes
    std::vector<iter_value_t<I>> seen;
    std::optional<iter_value_t<I>> dupe_elem;
    std::optional<iter_value_t<I>> pending;
    std::optional<iter_value_t<I>> cur_elem;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr duplicate_iterator() = default;

    GENEX_INLINE constexpr duplicate_iterator(
      I first, S last, Comp comp, Proj proj) :
      it(std::move(first)), st(std::move(last)),
      comp(std::move(comp)), proj(std::move(proj)) {
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto next() -> void {
      if (pending.has_value()) {
        cur_elem = std::move(pending);
        pending.reset();
        return;
      }

      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *cur_elem;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const duplicate_iterator &self, const duplicate_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const duplicate_iterator &self, duplicate_sentinel) -> bool {
      return self.it == self.st and not self.pending.has_value() and not self.cur_elem.has_value();
    }

  private:
    constexpr auto fwd_to_valid() -> void {
      cur_elem.reset();

      while (it != st) {
        // Inspect the current element (via projection)
        decltype(auto) cur_proj = meta::invoke(proj, *it);

        if (dupe_elem.has_value()) {
          if (meta::invoke(comp, cur_proj, meta::invoke(proj, *dupe_elem))) {
            cur_elem = *it;
            ++it;
            return;
          }
          ++it;
          continue;
        }

        // Search for the duplicate
        for (const auto &s : seen) {
          if (meta::invoke(comp, cur_proj, meta::invoke(proj, s))) {
            dupe_elem = s;
            cur_elem = s;
            pending = *it;
            ++it;
            return;
          }
        }

        seen.push_back(*it);
        ++it;
      }
    }
  };

  template <typename I, typename S, typename Comp, typename Proj>
  requires concepts::duplicate_checkable_iters<I, S, Comp, Proj>
  struct duplicate_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Comp comp;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr duplicate_view(
      I first, S last, Comp comp, Proj proj = {}) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) :
      it(std::move(first)), st(std::move(last)),
      comp(std::move(comp)), proj(std::move(proj)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return duplicate_iterator(it, st, comp, proj);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return duplicate_sentinel();
    }
  };
}

namespace genex::views {
  struct duplicates_fn : pipe_closure {
    template <
      typename I, typename S, typename Comp = operations::eq,
      typename Proj = meta::identity>
    requires detail::concepts::duplicate_checkable_iters<I, S, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Comp comp = {}, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(duplicate_view, I, S, Comp, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      return detail::impl::duplicate_view(
        std::move(first), std::move(last), std::move(comp), std::move(proj));
    }

    template <
      typename Rng, typename Comp = operations::eq,
      typename Proj = meta::identity>
    requires detail::concepts::duplicate_checkable_range<Rng, Comp, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Comp comp = {}, Proj proj = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(duplicate_view, iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj) and
      SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::duplicate_view(
        std::move(first), std::move(last), std::move(comp), std::move(proj));
    }

    template <typename Comp = operations::eq, typename Proj = meta::identity>
    requires closure_callable<Comp>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Comp comp, Proj proj = {}) const noexcept(
      SAFE_CTOR(duplicates_fn) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      return meta::bind_back(duplicates_fn{}, std::move(comp), std::move(proj));
    }
  };

  export inline constexpr duplicates_fn duplicates{};
}
