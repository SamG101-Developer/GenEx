#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::actions::detail {
    template <range Rng1, range Rng2> requires std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>
    auto do_concat(Rng1 &&rng1, Rng2 &&rng2) -> void {
        std::copy(
            iterators::begin(std::forward<Rng2>(rng2)),
            iterators::end(std::forward<Rng2>(rng2)),
            std::back_inserter(std::forward<Rng1>(rng1)));
    }
}


namespace genex::actions {
    /**
     * Modify a range in place, by concatenating another range to it. This places the second range at the end of the
     * first range, and iteration will continue through the second range after the first. Both ranges must have the
     * same value type.
     */
    DEFINE_ACTION(concat) {
        template <range Rng1, range Rng2> requires std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> void {
            FWD_TO_IMPL(detail::do_concat, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            MAKE_CLOSURE(rng2);
        }
    };

    EXPORT_GENEX_ACTION(concat);
}
