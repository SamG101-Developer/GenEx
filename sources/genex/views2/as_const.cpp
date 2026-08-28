module;
#include <genex/macros.hpp>

export module genex.views2.as_const;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept constable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::is_reference_v<iter_reference_t<I>>;

  template <typename Rng>
  concept constable_range =
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
  struct as_const_iterator {
    I it;

    using value_type = iter_value_t<I>;
    using reference_type = const iter_element_t<I>&;
    using reference = reference_type;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::iterator_traits<I>::iterator_category;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(as_const_iterator);

    GENEX_INLINE constexpr as_const_iterator() = default;

    GENEX_INLINE explicit constexpr as_const_iterator(I first) :
      it(std::move(first)) {
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      ++self.it;
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV {
      --self.it;
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return static_cast<reference_type>(*self.it);
    }

    GENEX_VIEW_ITER_EQ(as_const_iterator, as_const_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(as_const_iterator, as_const_sentinel<S>) {
      return self.it == that.st;
    }
  };

  template <typename I, typename S>
    requires concepts::constable_iters<I, S>
  struct as_const_view {
    I it;
    S st;

    GENEX_INLINE constexpr as_const_view(I first, S last) :
      it(std::move(first)), st(std::move(last)) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return as_const_iterator<I, S>(self.it);
    }

    template <typename Self>
    GENEX_ITER_END {
      if constexpr (std::same_as<I, S>) { return as_const_iterator<I, S>(self.st); }
      else { return as_const_sentinel<S>{self.st}; }
    }

    template <typename Self>
    GENEX_ITER_SIZE {
      return iterators::distance(self.it, self.st);
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