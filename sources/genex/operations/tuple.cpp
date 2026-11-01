module;
#include <genex/detail/macros.hpp>

export module genex.operations.tuple;
import std;

namespace genex {
  struct tuple_head_fn {
    template <typename... Ts>
    requires (sizeof...(Ts) > 0)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Ts &&... args) const noexcept(
      SAFE_NOEX(std::get<0>(std::forward_as_tuple(std::forward<Ts>(args)...)))) -> decltype(auto) {
      return std::get<0>(std::forward_as_tuple(std::forward<Ts>(args)...));
    }

    template <typename... Ts>
    requires (sizeof...(Ts) > 0)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> const &tpl) const noexcept(
      SAFE_NOEX(std::get<0>(tpl))) -> decltype(auto) {
      return std::get<0>(tpl);
    }

    template <typename... Ts>
    requires (sizeof...(Ts) > 0)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> &&tpl) const noexcept(
      SAFE_NOEX(std::get<0>(std::move(tpl)))) -> decltype(auto) {
      return std::get<0>(std::move(tpl));
    }
  };

  struct tuple_tail_fn {
    template <typename... Ts>
    requires (sizeof...(Ts) > 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Ts &&... args) const -> decltype(auto) {
      return std::apply([]<typename... T0>(auto &&, T0 &&... rest) {
        return std::make_tuple(std::forward<T0>(rest)...);
      }, std::forward_as_tuple(std::forward<Ts>(args)...));
    }

    template <typename... Ts>
    requires (sizeof...(Ts) > 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> const &tpl) const -> decltype(auto) {
      return std::apply([]<typename... T0>(auto &&, T0 &&... rest) {
        return std::make_tuple(std::forward<T0>(rest)...);
      }, tpl);
    }

    template <typename... Ts>
    requires (sizeof...(Ts) <= 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> const &) const noexcept(
      SAFE_NOEX(std::make_tuple())) -> decltype(auto) {
      return std::make_tuple();
    }

    template <typename... Ts>
    requires (sizeof...(Ts) > 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> &&tpl) const -> decltype(auto) {
      return std::apply([]<typename... T0>(auto &&, T0 &&... rest) {
        return std::make_tuple(std::forward<T0>(rest)...);
      }, std::move(tpl));
    }

    template <typename... Ts>
    requires (sizeof...(Ts) <= 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> &&) const noexcept(
      SAFE_NOEX(std::make_tuple())) -> decltype(auto) {
      return std::make_tuple();
    }
  };

  template <std::size_t N>
  struct get_fn {
    template <typename... Ts>
    requires (N < sizeof...(Ts))
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> &tpl) const noexcept(
      SAFE_NOEX(std::get<N>(tpl))) -> decltype(auto) {
      return std::get<N>(tpl);
    }

    template <typename... Ts>
    requires (N < sizeof...(Ts))
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> const &tpl) const noexcept(
      SAFE_NOEX(std::get<N>(tpl))) -> decltype(auto) {
      return std::get<N>(tpl);
    }

    template <typename... Ts>
    requires (N < sizeof...(Ts))
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Ts...> &&tpl) const noexcept(
      SAFE_NOEX(std::get<N>(std::move(tpl)))) -> decltype(auto) {
      return std::get<N>(std::move(tpl));
    }

    template <typename T, typename U>
    requires (N == 0 or N == 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::pair<T, U> &p) const noexcept(
      SAFE_NOEX(std::get<N>(p))) -> decltype(auto) {
      if constexpr (N == 0) { return (p.first); }
      else { return (p.second); }
    }

    template <typename T, typename U>
    requires (N == 0 or N == 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::pair<T, U> const &p) const noexcept(
      SAFE_NOEX(std::get<N>(p))) -> decltype(auto) {
      if constexpr (N == 0) { return (p.first); }
      else { return (p.second); }
    }

    template <typename T, typename U>
    requires (N == 0 or N == 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::pair<T, U> &&p) const noexcept(
      SAFE_NOEX(std::get<N>(std::move(p)))) -> decltype(auto) {
      if constexpr (N == 0) { return std::move(p.first); }
      else { return std::move(p.second); }
    }

    template <typename... Ts>
    requires (N < sizeof...(Ts)) and (sizeof...(Ts) > 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Ts &&... args) const noexcept(
      SAFE_NOEX(std::get<N>(std::forward_as_tuple(std::forward<Ts>(args)...)))) -> decltype(auto) {
      return std::get<N>(std::forward_as_tuple(std::forward<Ts>(args)...));
    }
  };

  /// Element "N" by value, for a tuple or pair that's about to be destroyed. An
  /// element that's a reference refers to something else, so stays a reference.
  template <std::size_t N>
  struct get_by_value_fn {
    template <typename Tpl, typename Element = std::tuple_element_t<N, std::remove_cvref_t<Tpl>>>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Tpl &&tpl) const noexcept(
      SAFE_CTOR(Element, decltype(std::get<N>(std::forward<Tpl>(tpl))))) -> Element {
      return std::get<N>(std::forward<Tpl>(tpl));
    }
  };

  export template <std::size_t N>
  inline constexpr get_fn<N> get{};
  export inline constexpr tuple_head_fn tuple_head{};
  export inline constexpr tuple_tail_fn tuple_tail{};
}

namespace genex::detail {
  /// How a view gets element "N" of what its iterator's "Reference" gives. A
  /// reference is to something that outlives the dereference, but anything else
  /// is a temporary, so a reference into it would dangle.
  export template <std::size_t N, typename Reference>
  using get_element_t = std::conditional_t<
    std::is_reference_v<Reference>, get_fn<N>, get_by_value_fn<N>>;
}
