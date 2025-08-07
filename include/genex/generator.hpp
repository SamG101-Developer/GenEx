#pragma once
#include <generator>


namespace genex {
    template <typename T>
    using generator = std::generator<T>;

    template <typename T>
    concept is_generator = requires(T g)
    {
        { g.begin() } -> std::same_as<typename generator<typename T::iterator::value_type>::iterator>;
        { g.end() } -> std::same_as<typename generator<typename T::iterator::value_type>::iterator>;
    };
}


// auto range = vector | genex::views::___
template <typename T, std::invocable<T> F>
auto operator|(T &&lhs, F &&rhs) {
    return std::forward<F>(rhs)(std::forward<T>(lhs));
}


// vector |= genex::actions::___
template <typename T, std::invocable<T> F>
auto operator|=(T &&lhs, F &&rhs) {
    return std::forward<F>(rhs)(std::forward<T>(lhs));
}
