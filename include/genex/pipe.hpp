#pragma once
#include <genex/macros.hpp>
#include <genex/meta.hpp>


// Usage: auto range = vector | genex::views::___
template <typename T, typename F> requires (std::invocable<F, T>)
GENEX_INLINE auto operator|(T &&lhs, F &&rhs) -> decltype(auto) {
    return genex::meta::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}


// Usage: vector |= genex::actions::___
template <typename T, typename F> requires (std::invocable<F, T>)
GENEX_INLINE auto operator|=(T &&lhs, F &&rhs) -> decltype(auto) {
    return genex::meta::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}
