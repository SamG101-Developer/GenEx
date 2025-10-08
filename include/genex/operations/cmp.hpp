#pragma once
#include <utility>

#define _GENEX_COMP_EQ std::forward<T>(a) == std::forward<U>(b)
#define _GENEX_COMP_NE std::forward<T>(a) != std::forward<U>(b)
#define _GENEX_COMP_LT std::forward<T>(a) < std::forward<U>(b)
#define _GENEX_COMP_LE std::forward<T>(a) <= std::forward<U>(b)
#define _GENEX_COMP_GT std::forward<T>(a) > std::forward<U>(b)
#define _GENEX_COMP_GE std::forward<T>(a) >= std::forward<U>(b)


namespace genex::operations {
    struct eq {
        template <typename T, typename U> requires std::equality_comparable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const noexcept(noexcept(_GENEX_COMP_EQ)) -> bool {
            return _GENEX_COMP_EQ;
        }
    };

    struct ne {
        template <typename T, typename U> requires std::equality_comparable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const noexcept(noexcept(_GENEX_COMP_NE)) -> bool {
            return _GENEX_COMP_NE;
        }
    };

    struct lt {
        template <typename T, typename U> requires std::totally_ordered_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const noexcept(noexcept(_GENEX_COMP_LT)) -> bool {
            return _GENEX_COMP_LT;
        }
    };

    struct le {
        template <typename T, typename U> requires std::totally_ordered_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const noexcept(noexcept(_GENEX_COMP_LE)) -> bool {
            return _GENEX_COMP_LE;
        }
    };

    struct gt {
        template <typename T, typename U> requires std::totally_ordered_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const noexcept(noexcept(_GENEX_COMP_GT)) -> bool {
            return _GENEX_COMP_GT;
        }
    };

    struct ge {
        template <typename T, typename U> requires std::totally_ordered_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const noexcept(noexcept(_GENEX_COMP_GE)) -> bool {
            return _GENEX_COMP_GE;
        }
    };
}


#undef _GENEX_COMP_EQ
#undef _GENEX_COMP_NE
#undef _GENEX_COMP_LT
#undef _GENEX_COMP_LE
#undef _GENEX_COMP_GT
#undef _GENEX_COMP_GE
