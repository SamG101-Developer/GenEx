module;
#include <genex/detail/macros.hpp>

export module genex.views.reverse;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.prev;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept reversible_iters =
    std::bidirectional_iterator<I> and
    std::sentinel_for<S, I>;

  template <typename Rng>
  concept reversible_range =
    non_dangling_range<Rng> and
    bidirectional_range<Rng> and
    reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
  struct reverse_iterator : genex::detail::iterator_ops<reverse_iterator<I, S>> {
    I it;
    S st;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr reverse_iterator() = default;

    GENEX_INLINE constexpr explicit reverse_iterator(I first, S last) :
      it(std::move(first)), st(std::move(last)) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      --it;
    }

    GENEX_INLINE constexpr auto prev() -> void requires std::bidirectional_iterator<decltype(it)> {
      ++it;
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *iterators::prev(it);
    }

    GENEX_INLINE friend constexpr auto operator==(const reverse_iterator &self, const reverse_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE constexpr auto distance_to(const reverse_iterator &that) const -> difference_type
      requires std::random_access_iterator<I> {
      return it - that.it;
    }
  };

  template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
  struct reverse_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr reverse_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return reverse_iterator<I, S>(st, it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return reverse_iterator<I, S>(it, st);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct reverse_fn {
    template <typename I, typename S>
      requires detail::concepts::reversible_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_IMPL_CTOR(reverse_view, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::reverse_view(std::move(first), std::move(last));
    }

    template <typename Rng>
      requires detail::concepts::reversible_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(reverse_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::reverse_view(std::move(first), std::move(last));
    }

    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(reverse_fn)) {
      return meta::bind_back(reverse_fn{});
    }
  };

  export inline constexpr reverse_fn reverse{};
}
