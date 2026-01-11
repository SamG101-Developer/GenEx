#pragma once


#ifndef NDEBUG
#define GENEX_ASSERT(E, cond)                               \
    do {                                                    \
        if (!(cond)) { throw E("Runtime assertion failed: " #cond); } \
    } while (0)
#else
#define GENEX_ASSERT(E, cond) \
    ((void)0)
#endif


#define GENEX_INLINE [[gnu::always_inline]] inline __attribute__((always_inline))

#define GENEX_NODISCARD [[nodiscard]]

#define GENEX_LIKELY [[likely]]#

#define GENEX_NO_UNIQUE_ADDRESS [[no_unique_address]]

#define GENEX_ITER_GUARD

#define GENEX_VIEW_CUSTOM_DEREF \
    GENEX_INLINE constexpr auto deref(this Self &&self) -> decltype(auto)

#define GENEX_VIEW_CUSTOM_NEXT \
    GENEX_INLINE constexpr auto next(this Self &&self) -> Self&

#define GENEX_VIEW_CUSTOM_PREV                 \
    requires std::bidirectional_iterator<decltype(std::declval<Self>().it)> \
    GENEX_INLINE constexpr auto prev(this Self &&self) -> Self&

#define GENEX_VIEW_ITER_EQ(SentType) \
    GENEX_INLINE constexpr auto operator==(this Self&& self, const SentType& that) -> bool

#define GENEX_ITER_BEGIN \
    GENEX_NODISCARD GENEX_INLINE constexpr auto begin(this Self&& self) noexcept -> auto

#define GENEX_ITER_END \
    GENEX_NODISCARD GENEX_INLINE constexpr auto end(this Self&& self) noexcept -> auto

#define GENEX_ITER_SIZE                                               \
    requires requires (Self &&s) { iterators::distance(s.it, s.st); } \
    GENEX_NODISCARD GENEX_INLINE constexpr auto size(this Self &&self)

#define GENEX_ITER_OPS_MINIMAL(Type)                                                   \
    GENEX_INLINE friend constexpr auto operator*(Type const &self) -> decltype(auto) { \
        return self.deref();                                                           \
    }                                                                                  \
                                                                                       \
    GENEX_INLINE friend constexpr auto operator++(Type &self) -> Type& {               \
        self.next();                                                                   \
        return self;                                                                   \
    }                                                                                  \
                                                                                       \
    GENEX_INLINE friend constexpr auto operator++(Type &self, int) -> Type {           \
        auto temp = self;                                                              \
        ++self;                                                                        \
        return temp;                                                                   \
    }

#define GENEX_ITER_OPS(Type)                                                                                  \
    GENEX_ITER_OPS_MINIMAL(Type)                                                                              \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator--(Type &self) -> Type&                                        \
    requires std::bidirectional_iterator<decltype(std::declval<Type>().it)> {                                 \
        self.prev();                                                                                          \
        return self;                                                                                          \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator--(Type &self, int) -> Type                                    \
    requires std::bidirectional_iterator<decltype(std::declval<Type>().it)> {                                 \
        auto temp = self;                                                                                     \
        --self;                                                                                               \
        return temp;                                                                                          \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator==(Type const &self, Type const &that) -> bool {               \
        return self.it == that.it;                                                                            \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator!=(Type const &self, Type const &that) -> bool {               \
        return not(self == that);                                                                             \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator+=(Type &self, Type::difference_type m) -> Type&               \
    requires std::random_access_iterator<decltype(std::declval<Type>().it)> {                                 \
        self.it += m;                                                                                         \
        return self;                                                                                         \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator+(Type const &self, Type::difference_type m) -> Type           \
    requires std::random_access_iterator<decltype(std::declval<Type>().it)> {                                 \
        auto temp = *self;                                                                                    \
        temp += m;                                                                                            \
        return temp;                                                                                          \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator+(Type const &self, Type const &that) -> Type::difference_type \
    requires std::random_access_iterator<decltype(std::declval<Type>().it)> {                                 \
        return self.it + that.it;                                                                             \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator-=(Type &self, Type::difference_type m) -> Type&               \
    requires std::random_access_iterator<decltype(std::declval<Type>().it)> {                                 \
        self.it -= m;                                                                                         \
        return self;                                                                                         \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator-(Type const &self, Type::difference_type m) -> Type           \
    requires std::random_access_iterator<decltype(std::declval<Type>().it)> {                                 \
        auto temp = *self;                                                                                    \
        temp -= m;                                                                                            \
        return temp;                                                                                          \
    }                                                                                                         \
                                                                                                              \
    GENEX_INLINE friend constexpr auto operator-(Type const &self, Type const &that) -> Type::difference_type \
    requires std::random_access_iterator<decltype(std::declval<Type>().it)> {                                 \
        return self.it - that.it;                                                                             \
    }
