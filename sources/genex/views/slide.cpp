module;
#include <genex/detail/macros.hpp>

export module genex.views.slide;
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
  concept slideable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::integral<Int>;

  template <typename Rng, typename Int>
  concept slideable_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    slideable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views::detail::impl {
  struct slide_sentinel {};

  template <typename I, typename S, typename Int>
  requires concepts::slideable_iters<I, S, Int>
  struct slide_iterator : genex::detail::iterator_ops<slide_iterator<I, S, Int>> {
    I it;
    I back;
    S st;

    using value_type = genex::subrange<I>;
    using reference = genex::subrange<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr slide_iterator() = default;

    GENEX_INLINE constexpr slide_iterator(I first, S last, Int window) :
      it(std::move(first)), back(it), st(std::move(last)) {
      back = iterators::next(
        it, static_cast<iter_difference_t<I>>(window) - 1, st);
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      ++back;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return genex::subrange(it, iterators::next(back));
    }

    GENEX_INLINE friend constexpr auto operator==(
      const slide_iterator &self, const slide_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const slide_iterator &self, slide_sentinel) -> bool {
      return self.back == self.st;
    }
  };

  template <typename I, typename S, typename Int>
  requires concepts::slideable_iters<I, S, Int>
  struct slide_view : view_base {
    I it;
    S st;
    Int window;

    GENEX_INLINE constexpr slide_view(I first, S last, Int window) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_CTOR(Int, Int&)) :
      it(std::move(first)), st(std::move(last)), window(window) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return slide_iterator<I, S, Int>(it, st, window);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return slide_sentinel();
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      const auto len = iterators::distance(it, st);
      const auto width = static_cast<std::ptrdiff_t>(window);
      return len < width ? std::ptrdiff_t{0} : len - width + 1;
    }
  };
}

namespace genex::views {
  struct slide_fn : pipe_closure {
    template <typename I, typename S, typename Int>
    requires detail::concepts::slideable_iters<I, S, Int>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, const Int window) const noexcept(
      SAFE_IMPL_CTOR(slide_view, I, S, Int) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(window > 0);
      return detail::impl::slide_view(
        std::move(first), std::move(last), window);
    }

    template <typename Rng, typename Int>
    requires detail::concepts::slideable_range<Rng, Int>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, const Int window) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(slide_view, iterator_t<Rng>, sentinel_t<Rng>, Int) and
      SAFE_MOVE(Int)) {
      GENEX_ASSERT(window > 0);
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::slide_view(
        std::move(first), std::move(last), window);
    }

    template <typename Int>
    requires std::integral<Int>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      const Int window) const noexcept(
      SAFE_CTOR(slide_fn) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(window > 0);
      return meta::bind_back(slide_fn{}, window);
    }
  };

  export inline constexpr slide_fn slide{};
}