#pragma once


#define GENEX_EXPORT_STRUCT(name) \
    inline constexpr name ## _fn name


#define GENEX_ASSERT(E, cond) \
    if (!(cond)) { throw E("Runtime assertion failed: " #cond); }


#define GENEX_PRAGMA(x) _Pragma(#x)


#define GENEX_MARK_AS_BROKEN(message) GENEX_PRAGMA(GCC warning message)
