#pragma once
#include <concepts>


namespace genex::strings {
    template <typename C>
    concept strict_char_like =
        std::same_as<std::remove_cv_t<C>, char> or
        std::same_as<std::remove_cv_t<C>, signed char> or
        std::same_as<std::remove_cv_t<C>, unsigned char>;

    template <typename C>
    concept wide_char_like =
        std::same_as<std::remove_cv_t<C>, wchar_t>;

    template <typename C>
    concept utf_char_like =
#if __cpp_char8_t >= 201811L
        std::same_as<std::remove_cv_t<C>, char8_t> or
#endif
        std::same_as<std::remove_cv_t<C>, char16_t> or
        std::same_as<std::remove_cv_t<C>, char32_t>;

    template <typename C>
    concept char_like =
        strict_char_like<C> or
        wide_char_like<C> or
        utf_char_like<C>;
}
