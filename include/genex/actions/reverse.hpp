#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::actions::detail {
    template <range Rng>
    auto do_reverse(Rng *rng) -> void {
        std::reverse(iterators::begin(*rng), iterators::end(*rng));
    }
}


namespace genex::actions {
    DEFINE_ACTION(sort) {
        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL(detail::do_reverse, &rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_ACTION(sort);
}
