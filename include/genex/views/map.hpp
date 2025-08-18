#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> F>
    auto do_map(I &&first, S &&last, F &&f) -> generator<std::invoke_result_t<F, iter_value_t<I>>> {
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(f), *first);
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> F>
    auto do_map(Rng &&rng, F &&f) -> generator<std::invoke_result_t<F, range_value_t<Rng>>> {
        for (auto &&x : rng) {
            co_yield std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(map) {
        DEFINE_OUTPUT_TYPE(map);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> F>
        constexpr auto operator()(I &&first, S &&last, F &&f) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_map, first, last, f);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> F>
        constexpr auto operator()(Rng &&rng, F &&f) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_map, rng, f);
        }

        template <typename F>
        constexpr auto operator()(F &&f) const -> auto {
            MAP_TO_BASE(f);
        }
    };

    EXPORT_GENEX_VIEW(map);
}
