module;
#include <genex/detail/macros.hpp>

export module genex.views.pairwise;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
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
    non_dangling_range<Rng> and
    forward_range<Rng> and
    pairwiseable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  struct pairwise_sentinel {};

  template <typename I, typename S>
  requires concepts::pairwiseable_iters<I, S>
  struct pairwise_iterator :
    genex::detail::iterator_ops<pairwise_iterator<I, S>> {
    I it;
    I snd;
    S st;

    using value_type = std::pair<iter_value_t<I>, iter_value_t<I>>;
    using reference = std::pair<iter_reference_t<I>, iter_reference_t<I>>;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr pairwise_iterator() = default;

    GENEX_INLINE constexpr pairwise_iterator(I first, S last) :
      it(std::move(first)), snd(it), st(std::move(last)) {
      snd = iterators::next(it, 1, st);
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      ++snd;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return reference(*it, *snd);
    }

    GENEX_INLINE friend constexpr auto operator==(
      const pairwise_iterator &self,
      const pairwise_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const pairwise_iterator &self,
      pairwise_sentinel) -> bool {
      return self.snd == self.st;
    }
  };

  template <typename I, typename S>
  requires concepts::pairwiseable_iters<I, S>
  struct pairwise_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr pairwise_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return pairwise_iterator<I, S>(it, st);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return pairwise_sentinel();
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      const auto len = iterators::distance(it, st);
      return len < 2 ? std::ptrdiff_t{0} : len - 1;
    }
  };
}

namespace genex::views {
  struct pairwise_fn : pipe_closure {
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
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(pairwise_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::pairwise_view(std::move(first), std::move(last));
    }
  };

  export inline constexpr pairwise_fn pairwise{};
}