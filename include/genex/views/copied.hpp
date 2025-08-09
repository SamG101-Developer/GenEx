#pragma once
#include <coroutine>
#include <functional>
#include <memory>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S>
auto do_copied(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
    for (auto it = first; it != last; ++it) {
        co_yield *it;
    }
}


template <range Rng>
auto do_copied(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        co_yield std::forward<decltype(x)>(x);
    }
}


template <range Rng> requires (unique_ptr<range_value_t<Rng>>)
auto do_copied(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        co_yield std::make_unique<decltype(x)>(std::forward<decltype(x)>(x));
    }
}


namespace genex::views {
    struct copied_fn final : detail::view_base {
        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_copied, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_copied, rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(copied);
}
