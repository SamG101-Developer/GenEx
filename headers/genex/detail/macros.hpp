#pragma once

#ifndef NDEBUG
#define GENEX_ASSERT(cond)                                                                                 \
  do {                                                                                                     \
    if (!(cond)) {                                                                                         \
      std::cerr << __FILE__ << ':' << __LINE__ << ": Runtime assertion failed: " << #cond << std::endl;    \
      std::abort();                                                                                        \
    }                                                                                                      \
  } while (0)
#else
#define GENEX_ASSERT(cond) \
  ((void)0)
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define GENEX_INLINE __forceinline
#else
#define GENEX_INLINE [[gnu::always_inline]] inline
#endif

// MSVC accepts the standard attribute but ignores it, to keep its ABI.
#if defined(_MSC_VER)
#define GENEX_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define GENEX_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#define SAFE_CTOR(T, ...) \
  std::is_nothrow_constructible_v<T __VA_OPT__(, __VA_ARGS__)>

#define SAFE_IMPL_CTOR(T, ...) \
  std::is_nothrow_constructible_v<::genex::views::detail::impl::T __VA_OPT__(<__VA_ARGS__>) __VA_OPT__(, __VA_ARGS__)>

#define SAFE_MOVE(T) \
  std::is_nothrow_move_constructible_v<T>

#define SAFE_SWAP(T, U) \
  std::is_nothrow_swappable_with_v<T, U>

#define SAFE_CALL(F, ...) \
  std::is_nothrow_invocable_v<F __VA_OPT__(, __VA_ARGS__)>

#define SAFE_NOEX(...) \
  noexcept(__VA_ARGS__)
