#pragma once
#include <coroutine>
#include <functional>
#include <genex/categories.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S, std::invocable<iter_value_t<I>> F> requires (
        categories::input_iterator<I> and
        std::indirectly_unary_invocable<F, I>)
    auto do_map(I &&first, S &&last, F &&f) -> generator<std::invoke_result_t<F, iter_value_t<I>>> {
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(f), *first);
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> F> requires (
        categories::input_range<Rng> and
        std::indirectly_unary_invocable<F, iterators::begin_t<Rng>>)
    auto do_map(Rng &&rng, F &&f) -> generator<std::invoke_result_t<F, range_value_t<Rng>>> {
        for (auto &&x : rng) {
            co_yield std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(map) {
        DEFINE_OUTPUT_TYPE(map);

        template <iterator I, sentinel_for<I> S, std::invocable<iter_value_t<I>> F> requires (
            categories::input_iterator<I> and
            std::indirectly_unary_invocable<F, I>)
        constexpr auto operator()(I &&first, S &&last, F &&f) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_map, first, last, f);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> F> requires (
            categories::input_range<Rng> and
            std::indirectly_unary_invocable<F, iterators::begin_t<Rng>>)
        constexpr auto operator()(Rng &&rng, F &&f) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_map, rng, f);
        }

        template <typename F>
        constexpr auto operator()(F &&f) const -> auto {
            MAKE_CLOSURE(f);
        }
    };

    EXPORT_GENEX_VIEW(map);
}
