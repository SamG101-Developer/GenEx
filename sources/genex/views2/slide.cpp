module;
#include <genex/macros.hpp>

export module genex.views2.slide;
export import genex.pipe;
import genex.concepts;
import genex.subrange;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept slideable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::integral<Int>;

  template <typename Rng, typename Int>
  concept slideable_range =
    forward_range<Rng> and
    slideable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views::detail::impl {
  struct slide_sentinel {};

  template <typename I, typename S, typename Int>
    requires concepts::slideable_iters<I, S, Int>
  struct slide_iterator {
    I it;
    I back;
    S st;

    using value_type = genex::subrange<I>;
    using reference_type = genex::subrange<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(slide_iterator);

    GENEX_INLINE constexpr slide_iterator() = default;

    GENEX_INLINE constexpr slide_iterator(I first, S last, Int window) :
      it(std::move(first)), back(it), st(std::move(last)) {
      back = iterators::next(it, static_cast<iter_difference_t<I>>(window) - 1, st);
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      ++self.it;
      ++self.back;
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV = delete;

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return genex::subrange(self.it, iterators::next(self.back));
    }

    GENEX_VIEW_ITER_EQ(slide_iterator, slide_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(slide_iterator, slide_sentinel) {
      GENEX_IGNORE(that);
      return self.back == self.st;
    }
  };

  template <typename I, typename S, typename Int>
    requires concepts::slideable_iters<I, S, Int>
  struct slide_view {
    I it;
    S st;
    Int window;

    GENEX_INLINE constexpr slide_view(I first, S last, Int window) :
      it(std::move(first)), st(std::move(last)), window(window) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return slide_iterator<I, S, Int>(self.it, self.st, self.window);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return slide_sentinel();
    }

    template <typename Self>
    GENEX_ITER_SIZE {
      const auto len = iterators::distance(self.it, self.st);
      const auto width = static_cast<std::ptrdiff_t>(self.window);
      return len < width ? std::ptrdiff_t{0} : len - width + 1;
    }
  };
}

namespace genex::views {
  struct slide_fn {
    template <typename I, typename S, typename Int>
      requires detail::concepts::slideable_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int window) const noexcept(
      SAFE_IMPL_CTOR(slide_view, I, S, Int) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(std::out_of_range, window > 0);
      return detail::impl::slide_view(std::move(first), std::move(last), window);
    }

    template <typename Rng, typename Int>
      requires detail::concepts::slideable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int window) const noexcept(
      SAFE_IMPL_CTOR(slide_view, iterator_t<Rng>, sentinel_t<Rng>, Int) and
      SAFE_MOVE(Int)) {
      GENEX_ASSERT(std::out_of_range, window > 0);
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::slide_view(std::move(first), std::move(last), window);
    }

    template <typename Int>
      requires std::integral<Int>
    GENEX_INLINE constexpr auto operator()(const Int window) const noexcept(
      SAFE_CTOR(slide_fn) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(std::out_of_range, window > 0);
      return meta::bind_back(slide_fn{}, window);
    }
  };

  export inline constexpr slide_fn slide{};
}