module;
#include <genex/detail/macros.hpp>

export module genex.detail.tuple;
import genex.concepts;
import std;

export namespace genex::detail {
  template <std::size_t I = 0, typename... Ts, typename... Us>
    requires (I <= sizeof...(Ts) and sizeof...(Ts) == sizeof...(Us))
  GENEX_INLINE constexpr auto any_iterator_finished(const std::tuple<Ts...> &starts,
    const std::tuple<Us...> &ends) -> bool {
    if constexpr (I < sizeof...(Ts)) {
      return std::get<I>(starts) == std::get<I>(ends) or any_iterator_finished<I
        + 1>(starts, ends);
    }
    else {
      return false;
    }
  }

  template <typename Tuple, std::size_t... Is>
  constexpr auto
  deref_tuple_impl(Tuple &t, std::index_sequence<Is...>) -> auto {
    return std::tuple<iter_reference_t<std::tuple_element_t<Is, Tuple>>...>{
      (*std::get<Is>(t))...
    };
  }

  template <typename Tuple>
  GENEX_INLINE constexpr auto deref_tuple(Tuple &t) -> auto {
    return deref_tuple_impl(
      t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple, std::size_t... Is>
  GENEX_INLINE constexpr auto advance_tuple_impl(Tuple &t,
    std::index_sequence<Is...>) -> void {
    ((++std::get<Is>(t)), ...);
  }

  template <typename Tuple>
  GENEX_INLINE constexpr auto advance_tuple(Tuple &t) -> void {
    advance_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple, typename Diff, std::size_t... Is>
  GENEX_INLINE constexpr auto advance_tuple_impl(Tuple &t, Diff n,
    std::index_sequence<Is...>) -> void {
    ((std::get<Is>(t) += n), ...);
  }

  template <typename Tuple, typename Diff>
  GENEX_INLINE constexpr auto advance_tuple(Tuple &t, Diff n) -> void {
    advance_tuple_impl(
      t, n, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple, std::size_t... Is>
  GENEX_INLINE constexpr auto retreat_tuple_impl(Tuple &t,
    std::index_sequence<Is...>) -> void {
    ((--std::get<Is>(t)), ...);
  }

  template <typename Tuple>
  GENEX_INLINE constexpr auto retreat_tuple(Tuple &t) -> void {
    retreat_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple>
    requires (std::tuple_size_v<Tuple> > 0)
  GENEX_INLINE constexpr auto distance_tuple(const Tuple &a, const Tuple &b) -> auto {
    return std::get<0>(a) - std::get<0>(b);
  }
}
