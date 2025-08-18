#pragma once
#include <utility>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::actions {
    template <typename Rng>
    concept has_member_insert = requires(Rng &&r) { r.insert(std::declval<range_value_t<Rng>>(), std::declval<std::size_t>()); };
}


namespace genex::actions::detail {
    template <typename Rng> requires has_member_insert<Rng>
    auto do_insert(Rng *rng, range_value_t<Rng> &&elem, const std::size_t pos) -> void {
        rng->insert(std::forward<range_value_t<Rng>>(elem), pos);
    }
}


namespace genex::actions {
    DEFINE_ACTION(insert) {
        template <typename Rng> requires has_member_insert<Rng>
        constexpr auto operator()(Rng &&r, range_value_t<Rng> &&elem) const -> void {
            FWD_TO_IMPL(detail::do_insert, r, elem);
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const -> auto {
            MAKE_CLOSURE(elem);
        }
    };

    EXPORT_GENEX_ACTION(insert);
}
