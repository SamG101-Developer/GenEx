module;
#include <functional>


export module genex.meta;

namespace genex::meta {
    export struct identity {
        template <typename T>
        auto operator()(T &&x) const -> T { return std::forward<T>(x); }
    };
}
