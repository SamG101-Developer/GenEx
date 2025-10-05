#pragma once


#define GENEX_EXPORT_STRUCT(name) \
    inline constexpr name ## _fn name


#define GENEX_ASSERT(E, cond) \
    if (!(cond)) { throw E("Runtime assertion failed: " #cond); }


#define GENEX_PRAGMA(x) _Pragma(#x)


#define GENEX_MARK_AS_BROKEN(message) GENEX_PRAGMA(GCC warning message)


#define GENEX_NO_ASAN __attribute__((no_sanitize_address))


#define GENEX_INLINE [[gnu::always_inline]] inline __attribute__((always_inline))


#define GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(con)     \
    using iterator_concept = con;                     \
    using iterator_category = iterator_concept;       \
    using difference_type = difference_type_selector_t<I>

#define GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(name, ...) \
    I it;                                               \
    S st;                                               \
                                                        \
    constexpr name() = default;

#define GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(iter, ...)               \
    GENEX_INLINE constexpr auto operator++(int)                       \
    -> iter requires std::forward_iterator<I> {                       \
        auto tmp = *this;                                             \
        ++it;                                                         \
        return tmp;                                                   \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator--() noexcept(noexcept(--it)) \
    -> iter& requires std::bidirectional_iterator<I> {                \
        --it;                                                         \
        return *this;                                                 \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator--(int)                       \
    -> iter requires std::bidirectional_iterator<I> {                 \
        auto tmp = *this;                                             \
        --it;                                                         \
        return tmp;                                                   \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator+=(                           \
    difference_type n) noexcept(noexcept(it += n))                    \
    -> iter& requires std::random_access_iterator<I> {                \
        it += n;                                                      \
        return *this;                                                 \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator+(                            \
    difference_type n) const noexcept(noexcept(it + n))               \
    -> iter requires std::random_access_iterator<I> {                 \
        return iter{it + n, __VA_ARGS__};                             \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator-=(                           \
    difference_type n) noexcept(noexcept(it -= n))                    \
    -> iter& requires std::random_access_iterator<I> {                \
        it -= n;                                                      \
        return *this;                                                 \
    }                                                                 \
                                                                      \
    GENEX_INLINE constexpr auto operator-(                            \
    difference_type n) const noexcept(noexcept(it - n))               \
    -> iter requires std::random_access_iterator<I> {                 \
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

#define GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(name)               \
    S s;                                                         \
                                                                 \
    GENEX_INLINE constexpr name() = default;                     \
                                                                 \
    GENEX_INLINE constexpr explicit name(                        \
        S s) noexcept(std::is_nothrow_move_constructible_v<S>) : \
        s(std::move(s)) {                                        \
    }

#define GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(iter, sent, ...)                                                                                    \
    template <typename I>                                                                                                                        \
    friend constexpr auto operator==(sent const &se, iter<I, S __VA_OPT__(, __VA_ARGS__)> const &it) noexcept(noexcept(it.it == se.s)) -> bool { \
        return it.it == se.s;                                                                                                                    \
    }                                                                                                                                            \
                                                                                                                                                 \
    template <typename I>                                                                                                                        \
    friend constexpr auto operator!=(sent const &se, iter<I, S __VA_OPT__(, __VA_ARGS__)> const &it) noexcept(noexcept(it.it != se.s)) -> bool { \
        return it.it != se.s;                                                                                                                    \
    }

#define GENEX_VIEW_VIEW_CTOR_DEFINITIONS(name) \
    V base_rng;                                \
                                               \
    GENEX_INLINE constexpr explicit name() = default;

#define GENEX_VIEW_VIEW_TYPE_DEFINITIONS(iter, sent, ...) \
    using iterator = iter<iterator_t<V>, sentinel_t<V> __VA_OPT__(, __VA_ARGS__)>;    \
    using sentinel = sent<sentinel_t<V> __VA_OPT__(, __VA_ARGS__)>;

#define GENEX_VIEW_VIEW_FUNC_DEFINITIONS(...)                                                                                        \
    GENEX_INLINE constexpr auto begin() noexcept(std::is_nothrow_copy_constructible_v<iterator_t<V>>) -> iterator {                  \
        return iterator{internal_begin(), internal_end() __VA_OPT__(, __VA_ARGS__)};                             \
    }                                                                                                                                \
                                                                                                                                     \
    GENEX_INLINE constexpr auto begin() const noexcept(std::is_nothrow_copy_constructible_v<iterator_t<V>>) -> iterator requires range<const V> { \
        return iterator{internal_begin(), internal_end() __VA_OPT__(, __VA_ARGS__)};                             \
    }                                                                                                                                \
                                                                                                                                     \
    GENEX_INLINE constexpr auto end() noexcept(std::is_nothrow_copy_constructible_v<sentinel_t<V>>) -> sentinel {                    \
        return sentinel{internal_end()};                                                                                   \
    }                                                                                                                                \
                                                                                                                                     \
    GENEX_INLINE constexpr auto end() const noexcept(std::is_nothrow_copy_constructible_v<sentinel_t<V>>) -> sentinel requires range<const V> {   \
        return sentinel{internal_end()};                                                                                   \
    }                                                                                                                                \
                                                                                                                                     \
    GENEX_INLINE constexpr auto size() const -> range_size_t<V> requires sized_range<V> {                                            \
        return operations::size(base_rng);                                                                                           \
    }                                                                                                                                \


#define GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE
    // GENEX_INLINE constexpr auto as_pointer_subrange() const {                                   \
    //     using elem_t = std::remove_reference_t<iter_reference_t<iterator>>;                                                        \
    //     if constexpr (sized_range<V>) {                                                                                            \
    //         auto *data = operations::data(base_rng);                                                                               \
    //         return std::ranges::subrange<elem_t>{data, data + operations::size(base_rng)};                                         \
    //     }                                                                                                                          \
    //     return std::ranges::subrange<elem_t>{operations::data(base_rng), operations::data(base_rng) + operations::size(base_rng)}; \
    // }


#define GENEX_ITERATOR_PROXY_ACCESS                                                   \
    if constexpr (std::is_pointer_v<I> || requires(const I& i) { i.operator->(); }) { \
        return it;                                                                    \
    } else {                                                                          \
        return std::addressof(operator*());                                           \
    }
