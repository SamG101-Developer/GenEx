#pragma once
#include <genex/macros.hpp>

#define EXPORT_GENEX_ALGORITHM(name) \
    EXPORT_GENEX_STRUCT(name)

#define DEFINE_ALGORITHM(name) \
    struct name ## _fn : detail::algorithm_fn_base


namespace genex::algorithms::detail {
    struct algorithm_fn_base {
        virtual ~algorithm_fn_base() = default;
    };
}
