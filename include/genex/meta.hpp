#pragma once
#include <utility>


namespace genex::meta {
    struct identity {
        template <typename T>
        constexpr auto operator()(T &&x) const -> T&& {
            return std::forward<T>(x);
        }
    };
}
