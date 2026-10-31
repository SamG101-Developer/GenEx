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

namespace genex::actions::detail::concepts {
  template <typename... Rngs>
  concept concatenatable_range =
    sizeof...(Rngs) > 0 and (input_range<Rngs> and ...) and
    requires { typename std::common_type_t<range_value_t<Rngs>...>; };
}

namespace genex::actions {
  struct concat_fn : pipe_closure {
    template <typename Rng1, typename Rng2>
    requires detail::concepts::concatenatable_range<Rng1, Rng2>
    [[nodiscard]] constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> Rng1 {
      using I = iterator_t<Rng2>;
      constexpr auto moves_elems = not (
        std::is_lvalue_reference_v<Rng2> or non_dangling_range<Rng2>);
      using Source = std::conditional_t<moves_elems, std::move_iterator<I>, I>;

      // Knowing the whole range up front means one allocation and one copy,
      // instead of one insert per element.
      if constexpr (std::same_as<I, sentinel_t<Rng2>>
        and std::derived_from<iter_category_t<I>, std::forward_iterator_tag>
        and has_member_insert<Rng1>) {
        // and requires(Source source) { rng1.insert(iterators::end(rng1), source, source); }) {
        rng1.insert(
          iterators::end(rng1),
          Source(iterators::begin(rng2)),
          Source(iterators::end(rng2)));
      }
      else {
        for (auto &&x : rng2) {
          if constexpr (moves_elems) {
            rng1 |= actions::insert(
              iterators::end(rng1), std::move(x));
          }
          else {
            rng1 |= actions::insert(
              iterators::end(rng1), std::forward<decltype(x)>(x));
          }
        }
      }
      return std::forward<Rng1>(rng1);
    }

    template <typename Rng1, typename Rng2, typename... Rngs>
    requires (
      detail::concepts::concatenatable_range<Rng1, Rng2, Rngs...>
      and sizeof...(Rngs) > 0)
    GENEX_INLINE constexpr auto operator()(
      Rng1 &&rng1, Rng2 &&rng2, Rngs &&... rngs) const -> Rng1 {
      (*this)(rng1, std::forward<Rng2>(rng2));
      return (*this)(std::forward<Rng1>(rng1), std::forward<Rngs>(rngs)...);
    }

    template <typename Rng2>
    requires detail::concepts::concatenatable_range<Rng2>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
      return genex::detail::bind_range_back(concat_fn{}, std::forward<Rng2>(rng2));
    }
  };

  /// The "concat" action concatenates from another range into this one. Used as
  /// "genex::actions::concat(self, that)". The ranges themselves must be an
  /// input range with common element types.
  export inline constexpr concat_fn concat{};
}
