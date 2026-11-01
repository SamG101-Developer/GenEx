module;
#include <genex/detail/macros.hpp>

export module genex.actions.concat;
export import genex.pipe;
import genex.actions.insert;
import genex.concepts;
import genex.detail.range_closure;
import genex.iterators.access;
import genex.meta;
import std;

namespace genex::actions::detail {
  /// Whether concatenating this range is the last use of its elements.
  template <typename Rng>
  inline constexpr bool moves_elems =
    not (std::is_lvalue_reference_v<Rng> or non_dangling_range<Rng>);

  template <typename Rng>
  using concat_source_t = std::conditional_t<
    moves_elems<Rng>, std::move_iterator<iterator_t<Rng>>, iterator_t<Rng>>;
}

namespace genex::actions::detail::concepts {
  template <typename... Rngs>
  concept concatenatable_range =
    sizeof...(Rngs) > 0 and (input_range<Rngs> and ...) and
    requires { typename std::common_type_t<range_value_t<Rngs>...>; };

  /// Whether the whole range can go in one call. Knowing it up front means one
  /// allocation and one copy, instead of one insert per element.
  template <typename Rng1, typename Rng2>
  concept bulk_concatenatable_range =
    concatenatable_range<Rng1, Rng2> and
    std::same_as<iterator_t<Rng2>, sentinel_t<Rng2>> and
    std::derived_from<iter_category_t<iterator_t<Rng2>>, std::forward_iterator_tag> and
    requires(Rng1 &rng1, concat_source_t<Rng2> source) {
      rng1.insert(iterators::end(rng1), source, source);
    };
}

namespace genex::actions {
  struct concat_fn : pipe_closure {
    /// Todo: Bulk insertion needs to use actions::insert for emplace/push?
    /// Overload for ranges that don't have a bulk "insert" overload, requiring
    /// one-by-one element insertion. Only applicable for 2 ranges.
    template <typename Rng1, typename Rng2>
    requires detail::concepts::concatenatable_range<Rng1, Rng2>
    constexpr auto operator()(
      Rng1 &&rng1, Rng2 &&rng2) const -> Rng1 {
      for (auto &&x : rng2) {
        if constexpr (detail::moves_elems<Rng2>) {
          rng1 |= actions::insert(iterators::end(rng1), std::move(x));
        }
        else {
          using X = decltype(x);
          rng1 |= actions::insert(iterators::end(rng1), std::forward<X>(x));
        }
      }
      return std::forward<Rng1>(rng1);
    }

    /// Overload for ranges that have a bulk "insert" overload, simplifying
    /// chaining of different actions. Only applicable for 2 ranges.
    template <typename Rng1, typename Rng2>
    requires detail::concepts::bulk_concatenatable_range<Rng1, Rng2>
    constexpr auto operator()(
      Rng1 &&rng1, Rng2 &&rng2) const -> Rng1 {
      using Source = detail::concat_source_t<Rng2>;
      rng1.insert(
        iterators::end(rng1),
        Source(iterators::begin(rng2)),
        Source(iterators::end(rng2)));
      return std::forward<Rng1>(rng1);
    }

    /// Overload for ranges that can route into any of the more specialised
    /// concat overloads, for any number of ranges.
    template <typename Rng1, typename Rng2, typename... Rngs>
    requires (
      detail::concepts::concatenatable_range<Rng1, Rng2, Rngs...>
      and sizeof...(Rngs) > 0)
    GENEX_INLINE constexpr auto operator()(
      Rng1 &&rng1, Rng2 &&rng2, Rngs &&... rngs) const -> Rng1 {
      (*this)(rng1, std::forward<Rng2>(rng2));
      return (*this)(std::forward<Rng1>(rng1), std::forward<Rngs>(rngs)...);
    }

    /// Form a bound call to this "concat" action when only one range is
    /// provided.
    template <typename Rng2>
    requires detail::concepts::concatenatable_range<Rng2>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng2 &&rng2) const {
      return genex::detail::bind_range_back(
        concat_fn{}, std::forward<Rng2>(rng2));
    }
  };

  /// The "concat" action concatenates from another range into this one. Used as
  /// "genex::actions::concat(self, that)". The ranges themselves must be an
  /// input range with common element types.
  export inline constexpr concat_fn concat{};
}
