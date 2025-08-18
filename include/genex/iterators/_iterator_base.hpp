#pragma once
#include <genex/macros.hpp>

#define EXPORT_GENEX_ITERATOR(name) \
    EXPORT_GENEX_STRUCT(name)

#define DEFINE_ITERATOR(name) \
    struct name ## _fn : detail::iterator_fn_base


namespace genex::iterators::detail {
    struct iterator_fn_base {
        virtual ~iterator_fn_base() = default;
    };
}
