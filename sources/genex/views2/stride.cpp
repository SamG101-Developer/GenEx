module;
#include <genex/macros.hpp>

export module genex.views2.stride;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept strideable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::integral<Int>;

  template <typename Rng, typename Int>
  concept strideable_range =
    forward_range<Rng> and
    strideable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views::detail::impl {
  struct stride_sentinel {};

  template <typename I, typename S, typename Int>
    requires concepts::strideable_iters<I, S, Int>
  struct stride_iterator {
    I it;
    S st;
    Int step;

    using value_type = iter_value_t<I>;
    using reference_type = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(stride_iterator);

    GENEX_INLINE constexpr stride_iterator() = default;

    GENEX_INLINE constexpr stride_iterator(I first, S last, Int step) :
      it(std::move(first)), st(std::move(last)), step(step) {
    }

    // The step is bounded by the sentinel, so overshooting the end of a range whose length is not a
    // multiple of the step lands exactly on the end rather than running past it.
    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      self.it = iterators::next(self.it, static_cast<iter_difference_t<I>>(self.step), self.st);
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV = delete;

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return *self.it;
    }

    GENEX_VIEW_ITER_EQ(stride_iterator, stride_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(stride_iterator, stride_sentinel) {
      GENEX_IGNORE(that);
      return self.it == self.st;
    }
  };

  template <typename I, typename S, typename Int>
    requires concepts::strideable_iters<I, S, Int>
  struct stride_view {
    I it;
    S st;
    Int step;

    GENEX_INLINE constexpr stride_view(I first, S last, Int step) :
      it(std::move(first)), st(std::move(last)), step(step) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return stride_iterator<I, S, Int>(self.it, self.st, self.step);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return stride_sentinel();
    }

    template <typename Self>
    GENEX_ITER_SIZE {
      return (iterators::distance(self.it, self.st) + self.step - 1) / self.step;
    }
  };
}

namespace genex::views {
  struct stride_fn {
    template <typename I, typename S, typename Int>
      requires detail::concepts::strideable_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(I first, S last, const Int step) const noexcept(
      SAFE_IMPL_CTOR(stride_view, I, S, Int) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(std::out_of_range, step > 0);
      return detail::impl::stride_view(std::move(first), std::move(last), step);
    }

    template <typename Rng, typename Int>
      requires detail::concepts::strideable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int step) const noexcept(
      SAFE_IMPL_CTOR(stride_view, iterator_t<Rng>, sentinel_t<Rng>, Int) and
      SAFE_MOVE(Int)) {
      GENEX_ASSERT(std::out_of_range, step > 0);
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::stride_view(std::move(first), std::move(last), step);
    }

    template <typename Int>
      requires std::integral<Int>
    GENEX_INLINE constexpr auto operator()(const Int step) const noexcept(
      SAFE_CTOR(stride_fn) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(std::out_of_range, step > 0);
      return meta::bind_back(stride_fn{}, step);
    }
  };

  export inline constexpr stride_fn stride{};
}