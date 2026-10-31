module;
#include <genex/detail/macros.hpp>

export module genex.algorithms.fold_right;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::algorithms::detail::concepts {
  template <
    typename I, typename S, typename Init, typename F,
    typename Acc = std::remove_cvref_t<Init>>
  concept right_foldable_iters =
    std::bidirectional_iterator<I> and
    std::sentinel_for<S, I> and
    std::movable<Acc> and
    std::constructible_from<Acc, Init> and
    std::invocable<F, iter_reference_t<I>, Acc> and
    std::convertible_to<std::invoke_result_t<F, iter_reference_t<I>, Acc>, Acc>;

  template <typename Rng, typename Init, typename F>
  concept right_foldable_range =
    bidirectional_range<Rng> and
    right_foldable_iters<iterator_t<Rng>, sentinel_t<Rng>, Init, F>;
}

namespace genex::algorithms::detail::impl {
  template <typename I, typename S, typename Init, typename F>
    requires concepts::right_foldable_iters<I, S, Init, F>
  constexpr auto do_fold_right(
    I first, S last, Init &&init, F f) -> std::remove_cvref_t<Init> {
    auto acc = std::remove_cvref_t<Init>(std::forward<Init>(init));
    while (first != last) {
      --last;
      acc = meta::invoke(f, *last, std::move(acc));
    }
    return acc;
  }
}

namespace genex {
  struct fold_right_fn {
    template <typename I, typename S, typename Init, typename F>
    requires algorithms::detail::concepts::right_foldable_iters<I, S, Init, F>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Init &&init, F f) const {
      return algorithms::detail::impl::do_fold_right(
        std::move(first), std::move(last),
        std::forward<Init>(init), std::move(f));
    }

    template <typename Rng, typename Init, typename F>
    requires algorithms::detail::concepts::right_foldable_range<Rng, Init, F>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Init &&init, F f) const {
      auto [first, last] = iterators::iter_pair(rng);
      return algorithms::detail::impl::do_fold_right(
        std::move(first), std::move(last),
        std::forward<Init>(init), std::move(f));
    }
  };

  export inline constexpr fold_right_fn fold_right{};
}
