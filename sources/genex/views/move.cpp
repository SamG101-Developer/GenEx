module;
#include <genex/detail/macros.hpp>

export module genex.views.move;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept movable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::movable<iter_value_t<I>>;

  template <typename Rng>
  concept movable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    movable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  template <typename I, typename S>
  requires concepts::movable_iters<I, S>
  struct move_iterator : genex::detail::adaptor_ops<move_iterator<I, S>> {
    I it;

    using value_type = iter_value_t<I>;
    using reference = iter_value_t<I>&&;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr move_iterator() = default;

    GENEX_INLINE constexpr explicit move_iterator(I first) :
      it(std::move(first)) {
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return std::move(*it);
    }
  };

  template <typename I, typename S>
  requires concepts::movable_iters<I, S>
  struct move_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr move_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return move_iterator<I, S>(it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return move_iterator<I, S>(st);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct move_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::movable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_IMPL_CTOR(move_view, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::move_view(std::move(first), std::move(last));
    }

    template <typename Rng>
    requires detail::concepts::movable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(move_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::move_view(std::move(first), std::move(last));
    }
  };

  export inline constexpr move_fn move{};
}
