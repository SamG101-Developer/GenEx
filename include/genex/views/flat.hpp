#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S> requires range<iter_value_t<I>>
auto do_flat(I &&first, S &&last) -> genex::generator<range_value_t<iter_value_t<I>>> {
    for (; first != last; ++first) {
        for (auto &&x : *first) {
            co_yield x;
        }
    }
}


template <range Rng> requires range<range_value_t<Rng>>
auto do_flat(Rng &&rng) -> genex::generator<range_value_t<range_value_t<Rng>>> {
    for (auto &&x : rng) {
        for (auto &&y : x) {
            co_yield std::forward<decltype(y)>(y);
        }
    }
}


namespace genex::views {
    struct flat_fn final : detail::view_base {
        template <iterator I, sentinel S> requires range<iter_value_t<I>>
        auto operator()(I &&first, S &&last) const -> generator<range_value_t<iter_value_t<I>>> {
            MAP_TO_IMPL(do_flat, first, last);
        }

        template <range Rng> requires range<range_value_t<Rng>>
        auto operator()(Rng &&rng) const -> generator<range_value_t<range_value_t<Rng>>> {
            MAP_TO_IMPL(do_flat, rng);
        }

        auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(flat);
}
