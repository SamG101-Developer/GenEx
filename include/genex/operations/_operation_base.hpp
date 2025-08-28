#pragma once
#include <genex/generator.hpp>
#include <genex/macros.hpp>


#define EXPORT_GENEX_OPERATION(name) \
    EXPORT_GENEX_STRUCT(name)

#define DEFINE_OPERATION(name) \
    struct name ## _fn : detail::operation_fn_base


namespace genex::operations::detail {
    struct operation_fn_base {
        virtual ~operation_fn_base() = default;
    };
}
