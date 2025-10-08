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


#define GENEX_ITER_GUARD \
    if (first == last) { co_return; }
