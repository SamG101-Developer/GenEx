#pragma once
#include <functional>

// Usage: auto range = vector | genex::views::___
template <typename T, typename F> requires (std::invocable<F, T>)
auto operator|(T &&lhs, F &&rhs) -> decltype(auto) {
    return std::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}


// Usage: vector |= genex::actions::___
template <typename T, typename F> requires (std::invocable<F, T>)
auto operator|=(T &&lhs, F &&rhs) -> decltype(auto) {
    return std::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}
