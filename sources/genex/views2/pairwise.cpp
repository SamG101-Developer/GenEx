module;
#include <genex/macros.hpp>

export module genex.views2.pairwise;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept pairwiseable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I>;

  template <typename Rng>
  concept pairwiseable_range =
    forward_range<Rng> and
    pairwiseable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  struct pairwise_sentinel {};

  template <typename I, typename S>
    requires concepts::pairwiseable_iters<I, S>
  struct pairwise_iterator {
    I it;
    I snd;
    S st;

    using value_type = std::pair<iter_value_t<I>, iter_value_t<I>>;
    using reference_type = std::pair<iter_reference_t<I>, iter_reference_t<I>>;
    using reference = reference_type;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(pairwise_iterator);

    GENEX_INLINE constexpr pairwise_iterator() = default;

    GENEX_INLINE constexpr pairwise_iterator(I first, S last) :
      it(std::move(first)), snd(it), st(std::move(last)) {
      snd = iterators::next(it, 1, st);
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      ++self.it;
      ++self.snd;
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV = delete;

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return reference_type(*self.it, *self.snd);
    }

    GENEX_VIEW_ITER_EQ(pairwise_iterator, pairwise_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(pairwise_iterator, pairwise_sentinel) {
      GENEX_IGNORE(that);
      return self.snd == self.st;
    }
  };

  template <typename I, typename S>
    requires concepts::pairwiseable_iters<I, S>
  struct pairwise_view {
    I it;
    S st;

    GENEX_INLINE constexpr pairwise_view(I first, S last) :
      it(std::move(first)), st(std::move(last)) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return pairwise_iterator<I, S>(self.it, self.st);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return pairwise_sentinel();
    }

    template <typename Self>
    GENEX_ITER_SIZE {
      const auto len = iterators::distance(self.it, self.st);
      return len < 2 ? std::ptrdiff_t{0} : len - 1;
    }
  };
}

namespace genex::views {
  struct pairwise_fn {
    template <typename I, typename S>
      requires detail::concepts::pairwiseable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_IMPL_CTOR(pairwise_view, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::pairwise_view(std::move(first), std::move(last));
    }

    template <typename Rng>
      requires detail::concepts::pairwiseable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_IMPL_CTOR(pairwise_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::pairwise_view(std::move(first), std::move(last));
    }

    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(pairwise_fn)) {
      return meta::bind_back(pairwise_fn{});
    }
  };

  export inline constexpr pairwise_fn pairwise{};
}