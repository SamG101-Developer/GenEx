#pragma once
#include <utility>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/operations/_operations_base.hpp>

using namespace genex::concepts;


namespace genex::operations {
    template <typename Rng>
    concept has_member_insert = requires(Rng &&r) { r.insert(std::declval<range_value_t<Rng>>(), std::declval<std::size_t>()); };

    struct insert_fn final : detail::operations_base {
        template <typename Rng> requires has_member_insert<Rng>
        constexpr auto operator()(Rng &&r, range_value_t<Rng> const &elem) const noexcept -> void {
            r.insert(std::forward<range_value_t<Rng>>(elem));
        }
    };

    EXPORT_GENEX_STRUCT(insert);
}
