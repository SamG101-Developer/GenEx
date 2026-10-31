module;
#include <genex/detail/macros.hpp>

export module genex.views.divide;
export import genex.pipe;
import genex.concepts;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.span;
import genex.subrange;
import std;

namespace genex::views::detail::concepts {
  template <std::size_t N, typename I, typename S>
  concept divisible_iters =
    N > 0 and
    std::forward_iterator<I> and
    std::sentinel_for<S, I>;

  template <std::size_t N, typename Rng>
  concept divisible_range =
    non_dangling_range<Rng> and
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
  constexpr auto divide_impl(
    I first,
    const iter_difference_t<I> total) -> std::array<divide_part_t<I, S>, N> {
    using part_t = divide_part_t<I, S>;
    using diff_t = iter_difference_t<I>;

    // Spread the remainder over the leading pieces,
    // so the sizes never differ by more than 1.
    const auto base = total / static_cast<diff_t>(N);
    const auto rem = total % static_cast<diff_t>(N);

    auto it = std::move(first);
    auto cut = [&](const std::size_t i) -> part_t {
      const auto n = base + (static_cast<diff_t>(i) < rem ? diff_t{1} : diff_t{0});
      auto lo = it;
      it = iterators::next(it, n);
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
  struct divide_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::divisible_iters<N, I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last) const {
      const auto total = static_cast<iter_difference_t<I>>(
        iterators::distance(first, last));
      return detail::impl::divide_impl<N, I, S>(
        std::move(first), total);
    }

    template <typename Rng>
    requires detail::concepts::divisible_range<N, Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const {
      auto [first, last] = iterators::iter_pair(rng);
      const auto total = [&] -> range_difference_t<Rng> {
        if constexpr (has_member_size<Rng>) { return static_cast<range_difference_t<Rng>>(rng.size()); }
        else { return static_cast<range_difference_t<Rng>>(iterators::distance(first, last)); }
      }();
      return detail::impl::divide_impl<N, iterator_t<Rng>, sentinel_t<Rng>>(
        std::move(first), total);
    }

    template <typename Rng>
    requires (not non_dangling_range<Rng>) and detail::concepts::divisible_range<N, Rng&>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const {
      using part_t = std::vector<range_value_t<Rng>>;
      auto parts = (*this)(rng);
      return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> std::array<part_t, N> {
        return std::array<part_t, N>{
          part_t(std::make_move_iterator(parts[Is].begin()), std::make_move_iterator(parts[Is].end()))...
        };
      }(std::make_index_sequence<N>{});
    }
  };

  export template <std::size_t N>
  inline constexpr divide_fn<N> divide{};
}
