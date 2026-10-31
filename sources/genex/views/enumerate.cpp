module;
#include <genex/detail/macros.hpp>

export module genex.views.enumerate;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept enumerable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I>;

  template <typename Rng>
  concept enumerable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    enumerable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  template <typename S>
  struct enumerate_sentinel {
    S st;
  };

  template <typename I, typename S>
  requires concepts::enumerable_iters<I, S>
  struct enumerate_iterator :
    genex::detail::adaptor_ops<enumerate_iterator<I, S>> {
    I it;
    std::size_t index = 0uz;

    using value_type = std::pair<std::size_t, iter_value_t<I>>;
    using reference = value_type;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_concept = iter_strength_t<I>;
    using iterator_category = legacy_iter_category_t<
      reference, iterator_concept>;

    GENEX_INLINE constexpr enumerate_iterator() = default;

    GENEX_INLINE explicit constexpr enumerate_iterator(I it) :
      it(std::move(it)) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      ++index;
    }

    GENEX_INLINE constexpr auto prev() -> void
    requires std::bidirectional_iterator<decltype(it)> {
      --it;
      --index;
    }

    GENEX_INLINE constexpr auto advance_by(
      const difference_type n) -> void
    requires std::random_access_iterator<I> {
      it += n;
      index += static_cast<std::size_t>(n);
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return std::make_pair(index, *it);
    }

    GENEX_INLINE friend constexpr auto operator==(
      const enumerate_iterator &self,
      const enumerate_sentinel<S> &that) -> bool {
      return self.it == that.st;
    }
  };

  template <typename I, typename S>
  requires concepts::enumerable_iters<I, S>
  struct enumerate_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr enumerate_view(I it, S st) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(it)), st(std::move(st)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return enumerate_iterator<I, S>{it};
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      if constexpr (std::same_as<I, S>) {
        return enumerate_iterator<I, S>{st};
      }
      else {
        return enumerate_sentinel<S>{st};
      }
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct enumerate_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::enumerable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last) const noexcept(
      SAFE_IMPL_CTOR(enumerate_view, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::enumerate_view(std::move(first), std::move(last));
    }

    template <typename Rng>
    requires detail::concepts::enumerable_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(enumerate_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::enumerate_view(std::move(first), std::move(last));
    }
  };

  export inline constexpr enumerate_fn enumerate{};
}
