module;
#include <genex/detail/macros.hpp>

export module genex.views.chunk;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.meta;
import genex.subrange;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept chunkable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::integral<Int>;

  template <typename Rng, typename Int>
  concept chunkable_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    chunkable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views::detail::impl {
  struct chunk_sentinel {};

  template <typename I, typename S, typename Int>
  requires concepts::chunkable_iters<I, S, Int>
  struct chunk_iterator : genex::detail::iterator_ops<chunk_iterator<I, S, Int>> {
    I it;
    S st;
    Int chunk_size;

    using value_type = genex::subrange<I>;
    using reference = genex::subrange<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr chunk_iterator() = default;

    GENEX_INLINE constexpr chunk_iterator(I first, S last, Int size) :
      it(std::move(first)), st(std::move(last)), chunk_size(size) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      it = iterators::next(
        it, static_cast<iter_difference_t<I>>(chunk_size), st);
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      auto end_it = it;
      for (Int i = 0; i < chunk_size and end_it != st; ++i) { ++end_it; }
      return genex::subrange(it, end_it);
    }

    GENEX_INLINE friend constexpr auto operator==(
      const chunk_iterator &self,
      const chunk_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const chunk_iterator &self,
      chunk_sentinel) -> bool {
      return self.it == self.st;
    }
  };

  /// Stepping by an offset keeps each chunk independent of the last, where
  /// clamping an iterator to the end ties them together with a conditional
  /// move.
  template <typename I, typename S, typename Int>
  requires concepts::chunkable_iters<I, S, Int> and
    std::random_access_iterator<I> and std::sized_sentinel_for<S, I>
  struct chunk_iterator<I, S, Int> :
    genex::detail::iterator_ops<chunk_iterator<I, S, Int>> {
    I it;
    iter_difference_t<I> offset;
    iter_difference_t<I> total;
    iter_difference_t<I> chunk_size;

    using value_type = genex::subrange<I>;
    using reference = genex::subrange<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr chunk_iterator() = default;

    GENEX_INLINE constexpr chunk_iterator(I first, S last, Int size) :
      it(std::move(first)), offset(0), total(last - it),
      chunk_size(static_cast<iter_difference_t<I>>(size)) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      offset += chunk_size;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      const auto first = it + offset;
      return genex::subrange(
        first, it + std::min(offset + chunk_size, total));
    }

    GENEX_INLINE friend constexpr auto operator==(
      const chunk_iterator &self,
      const chunk_iterator &that) -> bool {
      return std::min(self.offset, self.total) ==
        std::min(that.offset, that.total) and self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const chunk_iterator &self,
      chunk_sentinel) -> bool {
      return self.offset >= self.total;
    }
  };

  template <typename I, typename S, typename Int>
  requires concepts::chunkable_iters<I, S, Int>
  struct chunk_view : view_base {
    I it;
    S st;
    Int chunk_size;

    GENEX_INLINE constexpr chunk_view(I first, S last, Int size) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_CTOR(Int, Int&)) :
      it(std::move(first)), st(std::move(last)), chunk_size(size) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return chunk_iterator<I, S, Int>(it, st, chunk_size);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return chunk_sentinel();
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return (iterators::distance(it, st) + chunk_size - 1) / chunk_size;
    }
  };
}

namespace genex::views {
  struct chunk_fn : pipe_closure {
    template <typename I, typename S, typename Int>
    requires detail::concepts::chunkable_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(
      I first, S last, const Int size) const noexcept(
      SAFE_IMPL_CTOR(chunk_view, I, S, Int) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      return detail::impl::chunk_view(std::move(first), std::move(last), size);
    }

    template <typename Rng, typename Int>
    requires detail::concepts::chunkable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, const Int size) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(chunk_view, iterator_t<Rng>, sentinel_t<Rng>, Int) and
      SAFE_MOVE(Int)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::chunk_view(std::move(first), std::move(last), size);
    }

    template <typename Int>
    requires std::integral<Int>
    GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
      SAFE_CTOR(chunk_fn) and SAFE_MOVE(Int)) {
      return meta::bind_back(chunk_fn{}, n);
    }
  };

  export inline constexpr chunk_fn chunk{};
}
