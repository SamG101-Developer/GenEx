module;
#include <genex/detail/macros.hpp>

export module genex.views.cast_static;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename To, typename I, typename S>
  concept static_castable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    requires(iter_value_t<I> &&from) { static_cast<To>(from); };

  template <typename To, typename Rng>
  concept static_castable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    static_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  template <typename To, typename I, typename S>
  requires concepts::static_castable_iters<To, I, S>
  struct cast_static_iterator :
    genex::detail::adaptor_ops<cast_static_iterator<To, I, S>> {
    I it;

    using value_type = To;
    using reference = To;
    using difference_type = iter_difference_t<I>;
    using iterator_concept = iter_strength_t<I>;
    using iterator_category = legacy_iter_category_t<
      reference, iterator_concept>;

    using pointer = void;

    GENEX_INLINE constexpr cast_static_iterator() = default;

    GENEX_INLINE constexpr explicit cast_static_iterator(I first) :
      it(std::move(first)) {
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return static_cast<To>(*it);
    }
  };

  template <typename To, typename I, typename S>
    requires concepts::static_castable_iters<To, I, S>
  struct cast_static_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr cast_static_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return cast_static_iterator<To, I, S>(it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return cast_static_iterator<To, I, S>(st);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  template <typename To>
  struct cast_static_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::static_castable_iters<To, I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last) const noexcept(
      SAFE_CTOR(detail::impl::cast_static_view<To, I, S>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::cast_static_view<To, I, S>(
        std::move(first), std::move(last));
    }

    template <typename Rng>
    requires detail::concepts::static_castable_range<To, Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CTOR(
        detail::impl::cast_static_view<To, iterator_t<Rng>, sentinel_t<Rng>>,
        iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::cast_static_view<
        To, iterator_t<Rng>, sentinel_t<Rng>>(
        std::move(first), std::move(last));
    }
  };

  export template <typename To>
  inline constexpr cast_static_fn<To> cast_static{};
}
