module;
#include <genex/macros.hpp>

export module genex.views2.split;
export import genex.pipe;
import genex.algorithms.count;
import genex.concepts;
import genex.subrange;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename E>
  concept splittable_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirect_equivalence_relation<operations::eq, I, std::remove_cvref_t<E> const*>;

  template <typename Rng, typename E>
  concept splittable_range =
    forward_range<Rng> and
    splittable_iters<iterator_t<Rng>, sentinel_t<Rng>, E>;
}

namespace genex::views::detail::impl {
  struct split_sentinel {};

  template <typename I, typename S, typename E>
    requires detail::concepts::splittable_iters<I, S, E>
  struct split_iterator {
    I it;
    I field_end;
    S st;
    E elem;

    using value_type = genex::subrange<I>;
    using reference_type = genex::subrange<I>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(split_iterator);

    GENEX_INLINE constexpr split_iterator() = default;

    GENEX_INLINE constexpr split_iterator(I first, S last, E e) :
      it(std::move(first)), st(std::move(last)), elem(std::move(e)) {
      seek_field_end();
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      self.it = self.field_end;
      if (self.it != self.st) { ++self.it; }
      self.seek_field_end();
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV = delete;

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return genex::subrange(self.it, self.field_end);
    }

    GENEX_VIEW_ITER_EQ(split_iterator, split_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(split_iterator, split_sentinel) {
      GENEX_IGNORE(that);
      return self.it == self.st;
    }

  private:
    template <typename Self>
    GENEX_INLINE constexpr auto seek_field_end(this Self &&self) -> void {
      self.field_end = self.it;
      while (self.field_end != self.st and not operations::eq{}(*self.field_end, self.elem)) { ++self.field_end; }
    }
  };

  template <typename I, typename S, typename E>
    requires detail::concepts::splittable_iters<I, S, E>
  struct split_view {
    I it;
    S st;
    E elem;

    GENEX_INLINE constexpr split_view(I first, S last, E e) :
      it(std::move(first)), st(std::move(last)), elem(std::move(e)) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return split_iterator(self.it, self.st, self.elem);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return split_sentinel();
    }

    template <typename Self>
    GENEX_ITER_SIZE {
      auto elem_count = genex::count(self.it, self.st, self.elem);
      return elem_count == 0 ? 1 : elem_count + 1;
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
