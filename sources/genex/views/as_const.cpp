module;
#include <genex/detail/macros.hpp>

export module genex.views.as_const;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept constable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::is_reference_v<iter_reference_t<I>>;

  template <typename Rng>
  concept constable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    constable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  template <typename S>
  struct as_const_sentinel {
    S st;
  };

  template <typename I, typename S>
    requires concepts::constable_iters<I, S>
  struct as_const_iterator : genex::detail::adaptor_ops<as_const_iterator<I, S>> {
    I it;

    using value_type = iter_value_t<I>;
    using reference = const iter_element_t<I>&;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_concept = iter_strength_t<I>;
    using iterator_category = legacy_iter_category_t<reference, iterator_concept>;

    GENEX_INLINE constexpr as_const_iterator() = default;

    GENEX_INLINE explicit constexpr as_const_iterator(I first) :
      it(std::move(first)) {
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return static_cast<reference>(*it);
    }

    GENEX_INLINE friend constexpr auto operator==(
      const as_const_iterator &self, const as_const_sentinel<S> &that) -> bool {
      return self.it == that.st;
    }
  };

  template <typename I, typename S>
    requires concepts::constable_iters<I, S>
  struct as_const_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr as_const_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return as_const_iterator<I, S>(it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      if constexpr (std::same_as<I, S>) { return as_const_iterator<I, S>(st); }
      else { return as_const_sentinel<S>{st}; }
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct as_const_fn {
    template <typename I, typename S>
      requires detail::concepts::constable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_IMPL_CTOR(as_const_view, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::as_const_view(std::move(first), std::move(last));
    }

    template <typename Rng>
      requires detail::concepts::constable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(as_const_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::as_const_view(std::move(first), std::move(last));
    }

    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(as_const_fn)) {
      return meta::bind_back(as_const_fn{});
    }
  };

  export inline constexpr as_const_fn as_const{};
}