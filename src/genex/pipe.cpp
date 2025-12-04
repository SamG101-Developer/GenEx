module;
#include <genex/macros.hpp>

export module genex.pipe;
import genex.meta;
import std;


// Usage: auto range = vector | genex::views::___
export template <typename T, typename F> requires (std::invocable<F, T>)
GENEX_INLINE auto operator|(T &&lhs, F &&rhs) -> decltype(auto) {
    return genex::meta::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}


// Usage: vector |= genex::actions::___
export template <typename T, typename F> requires (std::invocable<F, T>)
GENEX_INLINE auto operator|=(T &&lhs, F &&rhs) -> decltype(auto) {
    return genex::meta::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}
