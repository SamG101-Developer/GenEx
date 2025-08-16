#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_intersperse(I &&first, S &&last, iter_value_t<I> const &separator) -> generator<iter_value_t<I>> {
        if (iterators::distance(first, last) == 0) { co_return; }
        co_yield *first;
        for (; first != last; iterators::advance(first)) {
            co_yield separator;
            co_yield *first;
        }
    }


    template <range Rng>
    auto do_intersperse(Rng &&rng, range_value_t<Rng> const &separator) -> generator<range_value_t<Rng>> {
        if (iterators::distance(iterators::begin(rng), iterators::end(rng)) == 0) { co_return; }
        co_yield *iterators::begin(rng);
        for (auto it = iterators::next(iterators::begin(rng)); it != iterators::end(rng); it = iterators::next(it)) {
            co_yield separator;
            co_yield *it;
        }
    }


    template <iterator I, sentinel S, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, iter_value_t<I>>)
    auto do_intersperse_with(I &&first, S &&last, F &&separator) -> generator<iter_value_t<I>> {
        if (iterators::distance(first, last) == 0) { co_return; }
        co_yield *first;
        for (; first != last; iterators::advance(first)) {
            co_yield std::invoke(std::forward<F>(separator));
            co_yield *first;
        }
    }


    template <range Rng, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, range_value_t<Rng>>)
    auto do_intersperse_with(Rng &&rng, F &&separator) -> generator<range_value_t<Rng>> {
        if (iterators::distance(iterators::begin(rng), iterators::end(rng)) == 0) { co_return; }
        co_yield *iterators::begin(rng);
        for (auto it = iterators::next(iterators::begin(rng)); it != iterators::end(rng); it = iterators::next(it)) {
            co_yield std::invoke(std::forward<F>(separator));
            co_yield *it;
        }
    }
}


namespace genex::views {
    struct intersperse_fn final : detail::view_base {
        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, iter_value_t<I> const &sep) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(detail::do_intersperse, first, last, sep);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> const &sep) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(detail::do_intersperse, rng, sep);
        }

        template <typename E>
        constexpr auto operator()(E &&sep) const -> decltype(auto) {
            MAP_TO_BASE(sep);
        }
    };

    struct intersperse_with_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, F &&sep) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(detail::do_intersperse_with, first, last, sep);
        }

        template <range Rng, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, F &&sep) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(detail::do_intersperse_with, rng, sep);
        }

        template <std::invocable F>
        constexpr auto operator()(F &&sep) const -> decltype(auto) {
            MAP_TO_BASE(sep);
        }
    };

    EXPORT_GENEX_STRUCT(intersperse);
    EXPORT_GENEX_STRUCT(intersperse_with);
}
