#pragma once
#include <tuple>

#include <genex/macros.hpp>


namespace genex::operations {
    template <std::size_t N>
    struct get_fn {
        template <typename... Ts>
        requires (N < sizeof...(Ts))
        GENEX_INLINE constexpr auto operator()(std::tuple<Ts...> &tpl) const -> decltype(auto) {
            return std::get<N>(tpl);
        }

        template <typename... Ts>
        requires (N < sizeof...(Ts))
        GENEX_INLINE constexpr auto operator()(std::tuple<Ts...> const &tpl) const -> decltype(auto) {
            return std::get<N>(tpl);
        }

        template <typename... Ts>
        requires (N < sizeof...(Ts))
        GENEX_INLINE constexpr auto operator()(std::tuple<Ts...> &&tpl) const -> decltype(auto) {
            return std::get<N>(std::move(tpl));
        }

        template <typename... Ts>
        requires (N < sizeof...(Ts))
        GENEX_INLINE constexpr auto operator()(std::tuple<Ts...> const &&tpl) const -> decltype(auto) {
            return std::get<N>(std::move(tpl));
        }

        template <typename... Ts>
        requires (N < sizeof...(Ts)) and (sizeof...(Ts) > 1)
        GENEX_INLINE constexpr auto operator()(Ts &&... args) const -> decltype(auto) {
            return std::get<N>(std::forward_as_tuple(std::forward<Ts>(args)...));
        }
    };

    template <std::size_t N>
    inline constexpr get_fn<N> get{};
}
