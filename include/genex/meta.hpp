#pragma once
#include <utility>

#include <genex/macros.hpp>


namespace genex::meta {
    struct identity {
        template <typename T>
        GENEX_INLINE constexpr auto operator()(T &&x) const -> T&& {
            return std::forward<T>(x);
        }
    };

    template <template <typename> typename Trait, typename... Ts>
    struct all_of : std::bool_constant<(Trait<Ts>::value && ...)> { };

    template <template <typename> typename Trait, typename... Ts>
    inline constexpr auto all_of_v = all_of<Trait, Ts...>::value;

    template <template <typename> typename Trait, typename... Ts>
    struct any_of : std::bool_constant<(Trait<Ts>::value || ...)> { };

    template <template <typename> typename Trait, typename... Ts>
    inline constexpr auto any_of_v = any_of<Trait, Ts...>::value;
}
