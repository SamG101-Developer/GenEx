#pragma once


#define COMMA ,


#define GENEX_EXPORT_STRUCT(name) \
    inline constexpr name ## _fn name


#define GENEX_ASSERT(E, cond) \
    if (!(cond)) { throw E("Runtime assertion failed: " #cond); }


#define GENEX_PRAGMA(x) _Pragma(#x)


#define GENEX_MARK_AS_BROKEN(message) GENEX_PRAGMA(GCC warning message)


#define GENEX_NO_ASAN __attribute__((no_sanitize_address))


#define GENEX_INLINE [[gnu::always_inline]] inline __attribute__((always_inline))


#define GENEX_NODISCARD [[nodiscard]]


// #define GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(con)     \
//     using iterator_concept = con;                     \
//     using iterator_category = iterator_concept;       \
//     using difference_type = difference_type_selector_t<I>

#define GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(iter, it, ...)           \
    GENEX_INLINE constexpr auto operator++(int)                       \
    -> iter requires std::forward_iterator<decltype(it)> {            \
        auto tmp = *this;                                             \
        ++it;                                                         \
        return tmp;                                                   \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator--() noexcept(noexcept(--it)) \
    -> iter& requires std::bidirectional_iterator<decltype(it)> {     \
        --it;                                                         \
        return *this;                                                 \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator--(int)                       \
    -> iter requires std::bidirectional_iterator<decltype(it)> {      \
        auto tmp = *this;                                             \
        --it;                                                         \
        return tmp;                                                   \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator+=(                           \
    difference_type n) noexcept(noexcept(it += n))                    \
    -> iter& requires std::random_access_iterator<decltype(it)> {     \
        it += n;                                                      \
        return *this;                                                 \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator+(                            \
    difference_type n) const noexcept(noexcept(it + n))               \
    -> iter requires std::random_access_iterator<decltype(it)> {      \
        return iter{it + n, __VA_ARGS__};                             \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator-=(                           \
    difference_type n) noexcept(noexcept(it -= n))                    \
    -> iter& requires std::random_access_iterator<decltype(it)> {     \
        it -= n;                                                      \
        return *this;                                                 \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator-(                            \
    difference_type n) const noexcept(noexcept(it - n))               \
    -> iter requires std::random_access_iterator<decltype(it)> {      \
        return iter{it - n, __VA_ARGS__};                             \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator==(                           \
    iter const &other) const noexcept(noexcept(it == other.it))       \
    -> bool {                                                         \
        return it == other.it;                                        \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator!=(                           \
    iter const &other) const noexcept(noexcept(it != other.it))       \
    -> bool {                                                         \
        return it != other.it;                                        \
    }


// op and deduction guide for iterator ctor
#define GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(iter, sent, ...) \
    GENEX_INLINE constexpr auto operator==(iter<__VA_ARGS__> const &it, sent const &se) -> bool


#define GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iter, sent, base, ...)                                                                \
    GENEX_INLINE constexpr auto begin() {                                                                                      \
        return iter{internal_begin(), internal_end() __VA_OPT__(, __VA_ARGS__)};                                               \
    }                                                                                                                          \
                                                                                                                               \
    GENEX_INLINE constexpr auto begin() const requires range<const decltype(base)> {                                           \
        return iter{internal_begin(), internal_end() __VA_OPT__(, __VA_ARGS__)};                                               \
    }                                                                                                                          \
                                                                                                                               \
    GENEX_INLINE constexpr auto end() {                                                                                        \
        return sent{};                                                                                                         \
    }                                                                                                                          \
                                                                                                                               \
    GENEX_INLINE constexpr auto end() const requires range<const decltype(base)> {                                             \
        return sent{};                                                                                                         \
    }                                                                                                                          \

    // GENEX_INLINE constexpr auto as_pointer_subrange() const {                                                                  \
    // using elem_t = std::remove_reference_t<iter_reference_t<iter>>;                                                        \
    // if constexpr (sized_range<V>) {                                                                                            \
    //     auto *data = operations::data(base_rng);                                                                               \
    //     return std::ranges::subrange<elem_t>{data, data + operations::size(base_rng)};                                         \
    // }                                                                                                                          \
    // return std::ranges::subrange<elem_t>{operations::data(base_rng), operations::data(base_rng) + operations::size(base_rng)}; \
}


    // GENEX_INLINE constexpr auto size() const                                                                     \
    //     -> range_size_t<decltype(base)> requires sized_range<decltype(base)> {                                   \
    //     return operations::size(base);                                                                           \ TODO: add constraint back
    // }


// #define GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE



// #define GENEX_ITERATOR_PROXY_ACCESS                                                   \
//     if constexpr (std::is_pointer_v<I> || requires(const I& i) { i.operator->(); }) { \
//         return it;                                                                    \
//     } else {                                                                          \
//         return std::addressof(operator*());                                           \
//     }
