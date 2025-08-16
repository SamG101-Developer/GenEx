#pragma once


namespace genex::strings {
    template <typename C>
    concept strict_char_like =
        std::same_as<std::remove_cv_t<C>, char> ||
        std::same_as<std::remove_cv_t<C>, signed char> ||
        std::same_as<std::remove_cv_t<C>, unsigned char>;

    template <typename C>
    concept wide_char_like =
        std::same_as<std::remove_cv_t<C>, wchar_t>;

    template <typename C>
    concept utf_char_like =
#if __cpp_char8_t >= 201811L
        std::same_as<std::remove_cv_t<C>, char8_t> ||
#endif
        std::same_as<std::remove_cv_t<C>, char16_t> ||
        std::same_as<std::remove_cv_t<C>, char32_t>;

    template <typename C>
    concept char_like =
        strict_char_like<C> ||
        wide_char_like<C> ||
        utf_char_like<C>;
}


namespace genex::strings::detail {
    struct string_base {
        virtual ~string_base() = default;
    };
}
