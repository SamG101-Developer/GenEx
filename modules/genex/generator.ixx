module;
#include <experimental/generator>


export module genex.generator;


namespace genex {
    export template <typename T>
    using generator = std::experimental::generator<T>;
}


export template <typename T, std::invocable<T> F>
auto operator|(T &&lhs, F &&rhs) {
    return std::forward<F>(rhs)(std::forward<T>(lhs));
}
