module;
#include <genex/detail/macros.hpp>

export module genex.views.split;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import genex.subrange;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename E>
  concept splittable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_equivalence_relation<operations::eq, I, std::remove_cvref_t<E> const*>;

  template <typename Rng, typename E>
  concept splittable_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    splittable_iters<iterator_t<Rng>, sentinel_t<Rng>, E>;
}

namespace genex::views::detail::impl {
  struct split_sentinel {};

  template <typename I, typename S, typename E>
    requires detail::concepts::splittable_iters<I, S, E>
  struct split_iterator : genex::detail::iterator_ops<split_iterator<I, S, E>> {
    I it;
    I field_end;
    S st;
    E elem;

    using value_type = genex::subrange<I>;
    using reference = genex::subrange<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr split_iterator() = default;

    GENEX_INLINE constexpr split_iterator(I first, S last, E e) :
      it(std::move(first)), st(std::move(last)), elem(std::move(e)) {
      seek_field_end();
    }

    GENEX_INLINE constexpr auto next() -> void {
      it = field_end;
      if (it != st) { ++it; }
      seek_field_end();
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return genex::subrange(it, field_end);
    }

    GENEX_INLINE friend constexpr auto operator==(const split_iterator &self, const split_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(const split_iterator &self, split_sentinel) -> bool {
      return self.it == self.st;
    }

  private:
    constexpr auto seek_field_end() -> void {
      field_end = it;
      while (field_end != st and not operations::eq{}(*field_end, elem)) { ++field_end; }
    }
  };

  template <typename I, typename S, typename E>
    requires detail::concepts::splittable_iters<I, S, E>
  struct split_view : view_base {
    I it;
    S st;
    E elem;

    GENEX_INLINE constexpr split_view(I first, S last, E e) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(E)) :
      it(std::move(first)), st(std::move(last)), elem(std::move(e)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return split_iterator(it, st, elem);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return split_sentinel();
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      // Every separator ends a field, and anything after the last one is a field of its own.
      auto fields = 0z;
      auto in_field = false;
      for (auto cur = it; cur != st; ++cur) {
        in_field = not operations::eq{}(*cur, elem);
        if (not in_field) { ++fields; }
      }
      return in_field ? fields + 1z : fields;
    }
  };
}

namespace genex::views {
  struct split_fn {
    template <typename I, typename S, typename E>
      requires detail::concepts::splittable_iters<I, S, E>
    GENEX_INLINE constexpr auto operator()(I first, S last, E elem) const noexcept(
      SAFE_IMPL_CTOR(split_view, I, S, E) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(E)) {
      return detail::impl::split_view(std::move(first), std::move(last), std::move(elem));
    }

    template <typename Rng, typename E>
      requires detail::concepts::splittable_range<Rng, E>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(split_view, iterator_t<Rng>, sentinel_t<Rng>, E) and
      SAFE_MOVE(E)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::split_view(std::move(first), std::move(last), std::move(elem));
    }

    template <typename E>
    GENEX_INLINE constexpr auto operator()(E elem) const noexcept(
      SAFE_CTOR(split_fn) and SAFE_MOVE(E)) {
      return meta::bind_back(split_fn{}, std::move(elem));
    }
  };

  export inline constexpr split_fn split{};
}
