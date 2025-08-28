#pragma once
#include <genex/generator.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>


#define EXPORT_GENEX_ACTION(name) \
    EXPORT_GENEX_STRUCT(name)

#define DEFINE_ACTION(name) \
    struct name ## _fn : detail::action_fn_base


namespace genex::actions::detail {
    struct action_fn_base {
        virtual ~action_fn_base() = default;
    };
}
