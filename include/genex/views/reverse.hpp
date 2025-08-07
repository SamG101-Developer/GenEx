#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;

template <iterator I, sentinel S>
auto do_reverse(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        co_yield *first;
    }
}

template <range Rng>
auto do_reverse(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
    for (auto it = genex::iterators::rbegin(rng); it != genex::iterators::rend(rng); ++it) {
        co_yield *it;
    }
}

namespace genex::views {
    struct reverse_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_reverse, first, last);
        }

        template <range Rng>
        auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_reverse, rng);
        }

        auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(reverse);
}
