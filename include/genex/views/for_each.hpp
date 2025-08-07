#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> F>
auto do_for_each(I &&first, S &&last, F &&f) -> void {
    for (; first != last; ++first) {
        std::invoke(std::forward<F>(f), std::forward<decltype(*first)>(*first));
    }
}


template <range Rng, std::invocable<range_value_t<Rng>> F>
auto do_for_each(Rng &&rng, F &&f) -> void {
    for (auto &&x : rng) {
        std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x));
    }
}


namespace genex::views {
    struct for_each_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> F>
        auto operator()(I &&first, S &&last, F &&f) const -> void {
            MAP_TO_IMPL(do_for_each, first, last, f);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> F>
        auto operator()(Rng &&rng, F &&f) const -> void {
            MAP_TO_IMPL(do_for_each, rng, f);
        }

        template <typename F>
        auto operator()(F &&f) const -> decltype(auto) {
            MAP_TO_BASE(f);
        }
    };

    EXPORT_GENEX_STRUCT(for_each);
}
