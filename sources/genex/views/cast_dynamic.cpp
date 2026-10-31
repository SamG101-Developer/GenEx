module;
#include <genex/detail/macros.hpp>

export module genex.views.cast_dynamic;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename To, typename I, typename S>
  concept dynamic_castable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::is_pointer_v<To> and
    requires(iter_reference_t<I> from) { dynamic_cast<To>(from); };

  template <typename To, typename Rng>
  concept dynamic_castable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    dynamic_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  struct cast_dynamic_sentinel {};

  template <typename To, typename I, typename S>
  requires concepts::dynamic_castable_iters<To, I, S>
  struct cast_dynamic_iterator : genex::detail::iterator_ops<cast_dynamic_iterator<To, I, S>> {
    I it;
    S st;
    mutable To cached = nullptr;

    using value_type = To;
    using reference = To;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr cast_dynamic_iterator() = default;

    GENEX_INLINE constexpr cast_dynamic_iterator(I first, S last) :
      it(std::move(first)), st(std::move(last)) {
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return cached;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const cast_dynamic_iterator &self,
      const cast_dynamic_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const cast_dynamic_iterator &self,
      cast_dynamic_sentinel) -> bool {
      return self.it == self.st;
    }

  private:
    constexpr auto fwd_to_valid() -> void {
      while (it != st) {
        update_cache();
        if (cached != nullptr) { break; }
        ++it;
      }
    }

    GENEX_INLINE constexpr auto update_cache() -> void {
      if (it != st) { cached = dynamic_cast<To>(*it); }
      else { cached = nullptr; }
    }
  };

  template <typename To, typename I, typename S>
  requires concepts::dynamic_castable_iters<To, I, S>
  struct cast_dynamic_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr cast_dynamic_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return cast_dynamic_iterator<To, I, S>{it, st};
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return cast_dynamic_sentinel{};
    }
  };
}

namespace genex::views {
  template <typename To>
  struct cast_dynamic_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::dynamic_castable_iters<To, I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_CTOR(detail::impl::cast_dynamic_view<To, I, S>, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::cast_dynamic_view<To, I, S>(
        std::move(first), std::move(last));
    }

    template <typename Rng>
    requires detail::concepts::dynamic_castable_range<To, Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CTOR(
        detail::impl::cast_dynamic_view<To, iterator_t<Rng>, sentinel_t<Rng>>,
        iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::cast_dynamic_view<
        To, iterator_t<Rng>, sentinel_t<Rng>>(
        std::move(first), std::move(last));
    }
  };

  export template <typename To>
  inline constexpr cast_dynamic_fn<To> cast_dynamic{};
}
