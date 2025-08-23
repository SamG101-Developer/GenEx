#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
        categories::input_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old>)
    auto do_filter(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                co_yield *first;
            }
        }
    }

    template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old> and
        std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
    auto do_filter(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) {
                co_yield std::forward<decltype(x)>(x);
            }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(filter) {
        DEFINE_OUTPUT_TYPE(filter);

        template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old>)
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_filter, first, last, pred, proj);
        }

        template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old> and
            std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_filter, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(pred, proj);
        }
    };

    EXPORT_GENEX_VIEW(filter);
}
