module;
#include <genex/macros.hpp>

export module genex.views2.divide;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.span;
import genex.subrange;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;

namespace genex::views::detail::concepts {
  template <std::size_t N, typename I, typename S>
  concept divisible_iters =
    N > 0 and
    std::forward_iterator<I> and
    std::sentinel_for<S, I>;

  template <std::size_t N, typename Rng>
  concept divisible_range =
    forward_range<Rng> and
    divisible_iters<N, iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  template <typename I, typename S>
  using divide_part_t = std::conditional_t<
    std::contiguous_iterator<I> and std::sized_sentinel_for<S, I>,
    genex::span<iter_element_t<I>>,
    genex::subrange<I, I>>;

  template <std::size_t N, typename I, typename S>
    requires concepts::divisible_iters<N, I, S>
  GENEX_INLINE constexpr auto divide_impl(I first, S last) -> std::array<divide_part_t<I, S>, N> {
    using part_t = divide_part_t<I, S>;
    using diff_t = iter_difference_t<I>;

    auto total = diff_t{0};
    if constexpr (std::sized_sentinel_for<S, I>) { total = last - first; }
    else { for (auto it = first; it != last; ++it) { ++total; } }

    // Spread the remainder over the leading pieces,
    // so the sizes never differ by more than 1.
    const auto base = total / static_cast<diff_t>(N);
    const auto rem = total % static_cast<diff_t>(N);

    auto it = std::move(first);
    auto cut = [&](const std::size_t i) -> part_t {
      const auto n = base + (static_cast<diff_t>(i) < rem ? diff_t{1} : diff_t{0});
      auto lo = it;
      it = iterators::next(it, n, last);
      return part_t(std::move(lo), it);
    };

    // Braced initializers are sequenced left-to-right,
    // so the pieces come out in range order.
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> std::array<part_t, N> {
      return std::array<part_t, N>{cut(Is)...};
    }(std::make_index_sequence<N>{});
  }
}

namespace genex::views {
  template <std::size_t N>
  struct divide_fn {
    template <typename I, typename S>
      requires detail::concepts::divisible_iters<N, I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_CTOR(detail::impl::divide_part_t<I, S>, I, I) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::divide_impl<N>(std::move(first), std::move(last));
    }

    template <typename Rng>
      requires detail::concepts::divisible_range<N, Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CTOR(detail::impl::divide_part_t<iterator_t<Rng>, sentinel_t<Rng>>,
        iterator_t<Rng>, iterator_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::divide_impl<N>(std::move(first), std::move(last));
    }

    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(divide_fn)) {
      return meta::bind_back(divide_fn{});
    }
  };

  export template <std::size_t N>
  inline constexpr divide_fn<N> divide{};
}
