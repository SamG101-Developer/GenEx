module;
#include <genex/macros.hpp>

export module genex.views2.chunk_by;
export import genex.pipe;
import genex.concepts;
import genex.subrange;
import genex.meta;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Pred>
  concept chunkable_by_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_binary_predicate<Pred, I, I>;

  template <typename Rng, typename Pred>
  concept chunkable_by_range =
    forward_range<Rng> and
    chunkable_by_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred>;
}

namespace genex::views::detail::impl {
  struct chunk_by_sentinel {};

  template <typename I, typename S, typename Pred>
    requires concepts::chunkable_by_iters<I, S, Pred>
  struct chunk_by_iterator {
    I it;
    I nxt;
    S st;
    GENEX_NO_UNIQUE_ADDRESS meta::box<Pred> pred;

    using value_type = genex::subrange<I>;
    using reference_type = genex::subrange<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(chunk_by_iterator);

    GENEX_INLINE constexpr chunk_by_iterator() = default;

    GENEX_INLINE constexpr chunk_by_iterator(I first, S last, Pred pred) :
      it(std::move(first)), nxt(it), st(std::move(last)), pred(std::move(pred)) {
      nxt = find_chunk_end(it);
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      self.it = self.nxt;
      self.nxt = self.find_chunk_end(self.it);
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV = delete;

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return genex::subrange(self.it, self.nxt);
    }

    GENEX_VIEW_ITER_EQ(chunk_by_iterator, chunk_by_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(chunk_by_iterator, chunk_by_sentinel) {
      GENEX_IGNORE(that);
      return self.it == self.st;
    }

  private:
    template <typename Self>
    GENEX_INLINE constexpr auto find_chunk_end(this Self &&self, I cur) -> I {
      if (cur == self.st) { return cur; }
      auto prev = cur;
      ++cur;
      while (cur != self.st and meta::invoke(*self.pred, *prev, *cur)) {
        prev = cur;
        ++cur;
      }
      return cur;
    }
  };

  template <typename I, typename S, typename Pred>
    requires concepts::chunkable_by_iters<I, S, Pred>
  struct chunk_by_view {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS Pred pred;

    GENEX_INLINE constexpr chunk_by_view(I first, S last, Pred pred) :
      it(std::move(first)), st(std::move(last)), pred(std::move(pred)) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return chunk_by_iterator<I, S, Pred>(self.it, self.st, self.pred);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return chunk_by_sentinel();
    }
  };
}

namespace genex::views {
  struct chunk_by_fn {
    template <typename I, typename S, typename Pred>
      requires detail::concepts::chunkable_by_iters<I, S, Pred>
    GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred) const noexcept(
      SAFE_IMPL_CTOR(chunk_by_view, I, S, Pred) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Pred)) {
      return detail::impl::chunk_by_view(std::move(first), std::move(last), std::move(pred));
    }

    template <typename Rng, typename Pred>
      requires detail::concepts::chunkable_by_range<Rng, Pred>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred) const noexcept(
      SAFE_IMPL_CTOR(chunk_by_view, iterator_t<Rng>, sentinel_t<Rng>, Pred) and
      SAFE_MOVE(Pred)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::chunk_by_view(std::move(first), std::move(last), std::move(pred));
    }

    template <typename Pred>
      requires (not range<Pred>)
    GENEX_INLINE constexpr auto operator()(Pred pred) const noexcept(
      SAFE_CTOR(chunk_by_fn) and SAFE_MOVE(Pred)) {
      return meta::bind_back(chunk_by_fn{}, std::move(pred));
    }
  };

  export inline constexpr chunk_by_fn chunk_by{};
}