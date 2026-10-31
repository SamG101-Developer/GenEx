module;
#include <genex/detail/macros.hpp>

export module genex.views.stride;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename Int>
  concept strideable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::integral<Int>;

  template <typename Rng, typename Int>
  concept strideable_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    strideable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views::detail::impl {
  struct stride_sentinel {};

  template <typename I, typename S, typename Int>
  requires concepts::strideable_iters<I, S, Int>
  struct stride_iterator :
    genex::detail::iterator_ops<stride_iterator<I, S, Int>> {
    I it;
    S st;
    Int step;

    static constexpr auto has_last_step =
      std::random_access_iterator<I>
      and std::same_as<I, S>
      and std::sized_sentinel_for<S, I>;

    GENEX_NO_UNIQUE_ADDRESS std::conditional_t<has_last_step, I, std::type_identity<I>> last_step{};

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr stride_iterator() = default;

    GENEX_INLINE constexpr stride_iterator(I first, S last, Int step) :
      it(std::move(first)), st(std::move(last)), step(step) {
      if constexpr (has_last_step) {
        const auto left = iterators::distance(it, st);
        last_step = it + (left > step ? left - step : 0);
      }
    }

    /// The step is bounded by the sentinel, so overshooting the end of a range
    /// whose length is not a multiple of the step lands exactly on the end
    /// rather than running past it.
    GENEX_INLINE constexpr auto next() -> void {
      if constexpr (has_last_step) {
        if (it < last_step) [[likely]] {
          it += static_cast<iter_difference_t<I>>(step);
        }
        else { it = st; }
      }
      else {
        it = iterators::next(it, static_cast<iter_difference_t<I>>(step), st);
      }
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const stride_iterator &self, const stride_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const stride_iterator &self, stride_sentinel) -> bool {
      return self.it == self.st;
    }
  };

  template <typename I, typename S, typename Int>
  requires concepts::strideable_iters<I, S, Int>
  struct stride_view : view_base {
    I it;
    S st;
    Int step;

    GENEX_INLINE constexpr stride_view(I first, S last, Int step) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_CTOR(Int, Int&)) :
      it(std::move(first)), st(std::move(last)), step(step) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return stride_iterator<I, S, Int>(it, st, step);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return stride_sentinel();
    }

    /// Every position is known up front, so there is no need to work out each
    /// one from the last.
    template <overwritable_with<iter_reference_t<I>> Out>
    requires std::random_access_iterator<I> and std::sized_sentinel_for<S, I>
    constexpr auto append_to(Out &out) const -> void {
      const auto count = static_cast<std::size_t>(size());
      out.resize(count);

      auto *dst = out.data();
      const auto width = static_cast<std::size_t>(step);
      for (auto i = std::size_t{0}; i < count; ++i) {
        dst[i] = it[static_cast<iter_difference_t<I>>(i * width)];
      }
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return (iterators::distance(it, st) + step - 1) / step;
    }
  };
}

namespace genex::views {
  struct stride_fn : pipe_closure {
    template <typename I, typename S, typename Int>
    requires detail::concepts::strideable_iters<I, S, Int>
    GENEX_INLINE constexpr auto operator()(
      I first, S last, const Int step) const noexcept(
      SAFE_IMPL_CTOR(stride_view, I, S, Int) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(step > 0);
      return detail::impl::stride_view(std::move(first), std::move(last), step);
    }

    template <typename Rng, typename Int>
    requires detail::concepts::strideable_range<Rng, Int>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, const Int step) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(stride_view, iterator_t<Rng>, sentinel_t<Rng>, Int) and
      SAFE_MOVE(Int)) {
      GENEX_ASSERT(step > 0);
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::stride_view(std::move(first), std::move(last), step);
    }

    template <typename Int>
    requires std::integral<Int>
    GENEX_INLINE constexpr auto operator()(
      const Int step) const noexcept(
      SAFE_CTOR(stride_fn) and SAFE_MOVE(Int)) {
      GENEX_ASSERT(step > 0);
      return meta::bind_back(stride_fn{}, step);
    }
  };

  export inline constexpr stride_fn stride{};
}