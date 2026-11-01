module;
#include <genex/detail/macros.hpp>

export module genex.iterators.next;
import genex.concepts;
import genex.iterators.advance;
import std;

namespace genex::iterators::detail::concepts {
  template <typename I, typename S = I>
  concept nextable_iters =
    std::input_or_output_iterator<I> and
    std::sentinel_for<S, I>;

  template <typename I, typename S>
  concept boundedly_nextable_iters =
    nextable_iters<I, S> and
    std::copyable<I>;

  template <typename I, typename S>
  concept commonly_nextable_iters =
    boundedly_nextable_iters<I, S> and
    std::same_as<I, S>;

  template <typename I, typename S>
  concept randomly_nextable_iters =
    boundedly_nextable_iters<I, S> and
    std::sized_sentinel_for<S, I> and
    std::random_access_iterator<I>;

  template <typename I, typename S>
  concept sized_nextable_iters =
    randomly_nextable_iters<I, S> and
    not std::same_as<I, S>;
}

namespace genex::iterators {
  struct next_fn {
    template <typename I>
    requires detail::concepts::nextable_iters<I> and std::copyable<I>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I it, const iter_difference_t<I> n = 1) const noexcept(
      SAFE_CALL(decltype(iterators::advance), I&, iter_difference_t<I>)) -> I {
      return iterators::advance(it, n);
    }

    template <typename I, typename S>
    requires detail::concepts::boundedly_nextable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I it, const iter_difference_t<I> n, const S bound) const -> I {
      if (n <= 0) { return it; }
      for (auto i = iter_difference_t<I>{0}; i < n and it != bound; ++i) {
        ++it;
      }
      return it;
    }

    template <typename I, typename S>
    requires detail::concepts::randomly_nextable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I it, const iter_difference_t<I> n, const S bound) const noexcept(
      SAFE_NOEX(it + n) and SAFE_NOEX(it + (bound - it)) and SAFE_MOVE(I)) -> I {
      if (n <= 0) { return it; }
      if (n < bound - it) [[likely]] { return it + n; }
      return it + (bound - it);
    }

    template <typename I, typename S>
    requires detail::concepts::boundedly_nextable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I it, S bound) const -> I {
      for (; it != bound; ++it) {}
      return it;
    }

    template <typename I, typename S>
    requires detail::concepts::commonly_nextable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I, S bound) const noexcept(
      SAFE_MOVE(S)) -> I {
      return bound;
    }

    template <typename I, typename S>
    requires detail::concepts::sized_nextable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I it, S bound) const noexcept(
      SAFE_NOEX(it + (bound - it))) -> I {
      return it + (bound - it);
    }
  };

  export inline constexpr next_fn next{};
}
