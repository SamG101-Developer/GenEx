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
    template <iterator I, sentinel_for<I> S, typename Old1, typename Old2 = iter_value_t<I>, std::invocable<Old2> Proj = meta::identity> requires (
        categories::input_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old1> and
        std::equality_comparable_with<iter_value_t<I>, Old2> and
        std::convertible_to<std::invoke_result_t<Proj, Old2>, iter_value_t<I>>)
    auto do_remove(I &&first, S &&last, Old1 &&elem, Proj &&proj) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Proj>(proj), *first) == elem) { continue; }
            co_yield *first;
        }
    }

    template <range Rng, typename Old1, typename Old2 = range_value_t<Rng>, std::invocable<Old2> Proj = meta::identity> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old1> and
        std::equality_comparable_with<range_value_t<Rng>, Old2> and
        std::convertible_to<std::invoke_result_t<Proj, Old2>, range_value_t<Rng>>)
    auto do_remove(Rng &&rng, Old1 &&elem, Proj &&proj) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == elem) { continue; }
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
        categories::input_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old>)
    auto do_remove_if(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
            co_yield *first;
        }
    }

    template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old> and
        std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
    auto do_remove_if(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { continue; }
            co_yield std::forward<decltype(x)>(x);
        }
    }
}


namespace genex::views {
    // todo: projection?
    DEFINE_VIEW(remove) {
        DEFINE_OUTPUT_TYPE(remove);

        template <iterator I, sentinel_for<I> S, typename Old1, typename Old2 = iter_value_t<I>, std::invocable<Old2> Proj = meta::identity> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old1> and
            std::equality_comparable_with<iter_value_t<I>, Old2> and
            std::convertible_to<std::invoke_result_t<Proj, Old2>, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, Old1 &&elem, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove, first, last, elem, proj);
        }

        template <range Rng, typename Old1, typename Old2 = range_value_t<Rng>, std::invocable<Old2> Proj = meta::identity> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old1> and
            std::equality_comparable_with<range_value_t<Rng>, Old2> and
            std::convertible_to<std::invoke_result_t<Proj, Old2>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, Old1 &&elem, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove, rng, elem, proj);
        }

        template <typename Old, typename Proj = meta::identity>
        constexpr auto operator()(Old &&elem, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(elem, proj);
        }
    };

    DEFINE_VIEW(remove_if) {
        DEFINE_OUTPUT_TYPE(remove_if);

        template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old>)
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove_if, first, last, pred, proj);
        }

        template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old> and
            std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove_if, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(pred, proj);
        }
    };


    EXPORT_GENEX_VIEW(remove);
    EXPORT_GENEX_VIEW(remove_if);
}
