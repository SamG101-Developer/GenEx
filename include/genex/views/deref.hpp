#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_deref(I &&first, S &&last) -> generator<deref_value_t<iter_value_t<I>>> {
        for (auto it = first; it != last; ++it) {
            co_yield **it;
        }
    }

    template <range Rng>
    auto do_deref(Rng &&rng) -> generator<deref_value_t<range_value_t<Rng>>> {
        for (auto &&x : rng) {
            co_yield *x;
        }
    }
}


namespace genex::views {
    struct deref_fn final : detail::view_base {
        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const -> generator<deref_value_t<iter_value_t<I>>> {
            MAP_TO_IMPL(detail::do_deref, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> generator<deref_value_t<range_value_t<Rng>>> {
            MAP_TO_IMPL(detail::do_deref, rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(deref);
}
