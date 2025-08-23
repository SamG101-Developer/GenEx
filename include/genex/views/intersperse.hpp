#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S, typename New> requires (
        categories::input_iterator<I> and
        std::convertible_to<New, iter_value_t<I>>)
    auto do_intersperse(I &&first, S &&last, New &&separator) -> generator<iter_value_t<I>> {
        co_yield *first;
        for (; first != last; ++first) {
            co_yield separator;
            co_yield *first;
        }
    }

    template <range Rng, typename New> requires (
        categories::input_range<Rng> and
        std::convertible_to<New, range_value_t<Rng>>)
    auto do_intersperse(Rng &&rng, New &&separator) -> generator<range_value_t<Rng>> {
        co_yield *iterators::begin(rng);
        for (auto it = iterators::next(iterators::begin(rng)); it != iterators::end(rng); ++it) {
            co_yield separator;
            co_yield *it;
        }
    }

    template <iterator I, sentinel_for<I> S, std::invocable F> requires (
        categories::input_iterator<I> and
        std::convertible_to<std::invoke_result_t<F>, iter_value_t<I>>)
    auto do_intersperse_with(I &&first, S &&last, F &&separator) -> generator<iter_value_t<I>> {
        co_yield *first;
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(separator));
            co_yield *first;
        }
    }

    template <range Rng, std::invocable F> requires (
        categories::input_range<Rng> and
        std::convertible_to<std::invoke_result_t<F>, range_value_t<Rng>>)
    auto do_intersperse_with(Rng &&rng, F &&separator) -> generator<range_value_t<Rng>> {
        co_yield *iterators::begin(rng);
        for (auto it = iterators::next(iterators::begin(rng)); it != iterators::end(rng); ++it) {
            co_yield std::invoke(std::forward<F>(separator));
            co_yield *it;
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(intersperse) {
        DEFINE_OUTPUT_TYPE(intersperse);

        template <iterator I, sentinel_for<I> S, typename New> requires (
            categories::input_iterator<I> and
            std::convertible_to<New, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, New &&sep) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_intersperse, first, last, sep);
        }

        template <range Rng, typename New> requires (
            categories::input_range<Rng> and
            std::convertible_to<New, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, New &&sep) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_intersperse, rng, sep);
        }

        template <typename New>
        constexpr auto operator()(New &&sep) const -> auto {
            MAKE_CLOSURE(sep);
        }
    };

    DEFINE_VIEW(intersperse_with) {
        DEFINE_OUTPUT_TYPE(intersperse_with);

        template <iterator I, sentinel_for<I> S, std::invocable F> requires (
            categories::input_iterator<I> and
            std::convertible_to<std::invoke_result_t<F>, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, F &&sep) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_intersperse_with, first, last, sep);
        }

        template <range Rng, std::invocable F> requires (
            categories::input_range<Rng> and
            std::convertible_to<std::invoke_result_t<F>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, F &&sep) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_intersperse_with, rng, sep);
        }

        template <std::invocable F>
        constexpr auto operator()(F &&sep) const -> auto {
            MAKE_CLOSURE(sep);
        }
    };

    EXPORT_GENEX_VIEW(intersperse);
    EXPORT_GENEX_VIEW(intersperse_with);
}
