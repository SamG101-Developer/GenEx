#pragma once


#ifndef NDEBUG
#define GENEX_ASSERT(E, cond)                                         \
    do {                                                              \
        if (!(cond)) { throw E("Runtime assertion failed: " #cond); } \
    } while (0)
#else
#define GENEX_ASSERT(E, cond) \
    ((void)0)
#endif


#define GENEX_INLINE [[gnu::always_inline]] inline __attribute__((always_inline))

#define GENEX_NODISCARD [[nodiscard]]

#define GENEX_LIKELY [[likely]]

#define GENEX_FORWARD_ITERATOR_OPERATIONS \
    using view_iterator_base::operator++; \
    using view_iterator_base::operator+;\
    using view_iterator_base::operator+=;\
    using view_iterator_base::operator--;\
    using view_iterator_base::operator-;\
    using view_iterator_base::operator-=;\
    using view_iterator_base::operator==;\
    using view_iterator_base::operator!=;\
    using view_iterator_base::operator*

#define GENEX_ITER_GUARD \
    if (first == last) { co_return; }

#define GENEX_VIEW_CUSTOM_DEREF \
    GENEX_INLINE constexpr auto deref(this Self &&self) -> decltype(auto)

#define GENEX_VIEW_CUSTOM_NEXT \
    GENEX_INLINE constexpr auto next(this Self &&self) -> Self&

#define GENEX_VIEW_CUSTOM_PREV                 \
    requires std::bidirectional_iterator<Self> \
    GENEX_INLINE constexpr auto prev(this Self &&self) -> Self&

#define GENEX_VIEW_ITER_EQ(SentType) \
    GENEX_INLINE constexpr auto operator==(this Self&& self, const SentType&) -> bool

#define GENEX_ITER_BEGIN \
    GENEX_NODISCARD GENEX_INLINE constexpr auto begin(this Self&& self) noexcept -> auto

#define GENEX_ITER_END \
    GENEX_NODISCARD GENEX_INLINE constexpr auto end(this Self&& self) noexcept -> auto

#define GENEX_ITER_SIZE                                        \
    requires requires (Self &&s) { iterators::distance(s.it, s.st); } \
    GENEX_NODISCARD GENEX_INLINE constexpr auto size(this Self &&self)

#define GENEX_ITER_EMPTY                                         \
    requires requires (Self &&s) { s.it == s.st; }                      \
    GENEX_NODISCARD GENEX_INLINE constexpr auto empty(this Self &&self) \
        -> bool
