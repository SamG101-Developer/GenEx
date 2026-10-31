module;
#include <genex/detail/macros.hpp>

export module genex.to_container;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.access;
import genex.iterators.advance;
import genex.iterators.iter_pair;
import std;

namespace genex::detail {
  /**
   * Hands a container the number of elements to expect, so it can allocate once and fill in a single loop. A view that
   * returns prvalues has to call itself an input iterator, which is right everywhere except here, where the size is
   * known anyway and each element is only read once.
   */
  template <typename I>
  struct sized_fill_iterator : iterator_ops<sized_fill_iterator<I>> {
    I it;
    std::ptrdiff_t left = 0;

    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *it;
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      --left;
    }

    GENEX_INLINE constexpr auto prev() -> void requires std::bidirectional_iterator<I> {
      --it;
      ++left;
    }

    // Standard libraries are free to use any of this once told it's random access, even if it means walking.
    GENEX_INLINE constexpr auto advance_by(const difference_type n) -> void {
      iterators::advance(it, static_cast<iter_difference_t<I>>(n));
      left -= n;
    }

    GENEX_INLINE constexpr auto distance_to(const sized_fill_iterator &that) const -> difference_type {
      return left - that.left;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const sized_fill_iterator &self, const sized_fill_iterator &that) -> bool {
      return self.left == that.left;
    }
  };
}

namespace genex {
  // Whether a range constructor can size itself up front, in which case nothing here is going to be faster.
  template <typename Rng>
  concept range_constructor_sizes =
    std::same_as<iterator_t<Rng>, sentinel_t<Rng>> and
    std::derived_from<typename std::iterator_traits<iterator_t<Rng>>::iterator_category, std::forward_iterator_tag>;

  template <typename Out, typename Rng>
  concept reserve_pushable =
    has_member_size<Rng> and
    has_member_reserve<Out> and
    has_member_push_back<Out> and
    (not range_constructor_sizes<Rng>);

  // A view that knows a faster way to write itself out than being iterated over.
  template <typename Out, typename Rng>
  concept self_appending = requires(Rng &rng, Out &out) { rng.append_to(out); };

  export template <template <typename> typename Out, typename Rng>
    requires input_range<Rng>
    and (not reserve_pushable<Out<range_value_t<Rng>>, Rng>)
    and (not self_appending<Out<range_value_t<Rng>>, Rng>)
    and std::copyable<range_value_t<Rng>> and requires(Rng &&rng)
    {
      Out<range_value_t<Rng>>(iterators::begin(rng), iterators::end(rng));
    }

  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
    return Out<range_value_t<Rng>>(iterators::begin(rng), iterators::end(rng));
  }

  export template <typename Out, typename Rng>
    requires input_range<Rng>
    and (not reserve_pushable<Out, Rng>)
    and (not self_appending<Out, Rng>)
    and std::copyable<range_value_t<Rng>> and requires(Rng &&rng) { Out(iterators::begin(rng), iterators::end(rng)); }

  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out {
    return Out(iterators::begin(rng), iterators::end(rng));
  }

  template <typename Out, typename Rng>
  concept bulk_fillable =
    (not self_appending<Out, Rng>) and
    reserve_pushable<Out, Rng> and
    std::copy_constructible<iterator_t<Rng>> and
    std::constructible_from<Out, detail::sized_fill_iterator<iterator_t<Rng>>, detail::sized_fill_iterator<iterator_t<Rng>>>;

  template <typename Out, typename Rng>
  GENEX_INLINE constexpr auto bulk_fill(Rng &rng) -> Out {
    using fill_t = detail::sized_fill_iterator<iterator_t<Rng>>;
    const auto size = static_cast<std::ptrdiff_t>(rng.size());
    auto first = iterators::begin(rng);
    return Out(fill_t{{}, first, size}, fill_t{{}, first, 0});
  }

  template <typename Out, typename Rng>
  GENEX_INLINE constexpr auto self_append(Rng &rng) -> Out {
    auto out = Out();
    rng.append_to(out);
    return out;
  }

  export template <template <typename> typename Out, typename Rng>
    requires input_range<Rng> and self_appending<Out<range_value_t<Rng>>, Rng>
  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
    return self_append<Out<range_value_t<Rng>>>(rng);
  }

  export template <typename Out, typename Rng>
    requires input_range<Rng> and self_appending<Out, Rng>
  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out {
    return self_append<Out>(rng);
  }

  export template <template <typename> typename Out, typename Rng>
    requires input_range<Rng> and bulk_fillable<Out<range_value_t<Rng>>, Rng>
  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
    return bulk_fill<Out<range_value_t<Rng>>>(rng);
  }

  export template <typename Out, typename Rng>
    requires input_range<Rng> and bulk_fillable<Out, Rng>
  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out {
    return bulk_fill<Out>(rng);
  }

  export template <template <typename> typename Out, typename Rng>
    requires input_range<Rng>
  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
    Out<range_value_t<Rng>> out;
    auto [first, last] = iterators::iter_pair(rng);
    if constexpr (has_member_size<Rng> and has_member_reserve<Out<range_value_t<Rng>>>) {
      out.reserve(static_cast<std::size_t>(rng.size()));
    }
    else if constexpr (has_member_reserve_hint<Rng> and has_member_reserve<Out<range_value_t<Rng>>>) {
      out.reserve(static_cast<std::size_t>(rng.reserve_hint()));
    }
    for (; first != last; ++first) {
      out.push_back(*first);
    }
    return out;
  }

  export template <typename Out, typename Rng>
    requires input_range<Rng>
  GENEX_INLINE constexpr auto to_base_fn(Rng &&rng) -> Out {
    Out out;
    auto [first, last] = iterators::iter_pair(rng);
    if constexpr (has_member_size<Rng> and has_member_reserve<Out>) {
      out.reserve(static_cast<std::size_t>(rng.size()));
    }
    else if constexpr (has_member_reserve_hint<Rng> and has_member_reserve<Out>) {
      out.reserve(static_cast<std::size_t>(rng.reserve_hint()));
    }
    for (; first != last; ++first) {
      out.push_back(*first);
    }
    return out;
  }

  export template <template <typename...> typename Out>
  GENEX_INLINE constexpr auto to() -> auto {
    return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
      return to_base_fn<Out>(std::forward<Rng>(rng));
    };
  }

  export template <typename Out>
  GENEX_INLINE constexpr auto to() -> auto {
    return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
      return to_base_fn<Out>(std::forward<Rng>(rng));
    };
  }
}
