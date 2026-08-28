module;
#include <genex/macros.hpp>

export module genex.views2.unique;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Comp, typename Proj>
  concept uniqueable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirectly_comparable<I, I, Comp, Proj, Proj>;

  template <typename Rng, typename Comp, typename Proj>
  concept uniqueable_range =
    forward_range<Rng> and
    uniqueable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}

namespace genex::views::detail::impl {
  struct unique_sentinel {};

  template <typename I, typename S, typename Comp, typename Proj>
    requires concepts::uniqueable_iters<I, S, Comp, Proj>
  struct unique_iterator {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS meta::box<Comp> comp;
    GENEX_NO_UNIQUE_ADDRESS meta::box<Proj> proj;

    using value_type = iter_value_t<I>;
    using reference_type = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(unique_iterator);

    GENEX_INLINE constexpr unique_iterator() = default;

    GENEX_INLINE constexpr unique_iterator(I first, S last, Comp comp, Proj proj) :
      it(std::move(first)), st(std::move(last)),
      comp(std::move(comp)), proj(std::move(proj)) {
    }

    // Every element skipped here compares equal to the one just yielded, so comparing each candidate
    // against that single element is enough for an equivalence relation -- no need to re-anchor.
    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      auto prev = self.it;
      ++self.it;
      while (self.it != self.st and meta::invoke(
        *self.comp, meta::invoke(*self.proj, *self.it), meta::invoke(*self.proj, *prev))) { ++self.it; }
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV = delete;

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return *self.it;
    }

    GENEX_VIEW_ITER_EQ(unique_iterator, unique_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(unique_iterator, unique_sentinel) {
      GENEX_IGNORE(that);
      return self.it == self.st;
    }
  };

  template <typename I, typename S, typename Comp, typename Proj>
    requires concepts::uniqueable_iters<I, S, Comp, Proj>
  struct unique_view {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Comp comp;
    GENEX_NO_UNIQUE_ADDRESS Proj proj;

    GENEX_INLINE constexpr unique_view(I first, S last, Comp comp, Proj proj = {}) :
      it(std::move(first)), st(std::move(last)),
      comp(std::move(comp)), proj(std::move(proj)) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return unique_iterator<I, S, Comp, Proj>(self.it, self.st, self.comp, self.proj);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return unique_sentinel();
    }
  };
}

namespace genex::views {
  struct unique_fn {
    template <typename I, typename S, typename Comp = operations::eq, typename Proj = meta::identity>
      requires detail::concepts::uniqueable_iters<I, S, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(I first, S last, Comp comp = {}, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(unique_view, I, S, Comp, Proj) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      return detail::impl::unique_view(std::move(first), std::move(last), std::move(comp), std::move(proj));
    }

    template <typename Rng, typename Comp = operations::eq, typename Proj = meta::identity>
      requires detail::concepts::uniqueable_range<Rng, Comp, Proj>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const noexcept(
      SAFE_IMPL_CTOR(unique_view, iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj) and
      SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::unique_view(std::move(first), std::move(last), std::move(comp), std::move(proj));
    }

    template <typename Comp = operations::eq, typename Proj = meta::identity>
      requires (not range<Comp>)
    GENEX_INLINE constexpr auto operator()(Comp comp = {}, Proj proj = {}) const noexcept(
      SAFE_CTOR(unique_fn) and SAFE_MOVE(Comp) and SAFE_MOVE(Proj)) {
      return meta::bind_back(unique_fn{}, std::move(comp), std::move(proj));
    }
  };

  export inline constexpr unique_fn unique{};
}