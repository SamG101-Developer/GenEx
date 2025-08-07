module;
#include <coroutine>
#include <functional>
#include <genex/macros.hpp>

export module genex.views.intersperse;
export import genex.generator;
import genex.concepts;
import genex.iterators.advance;
import genex.iterators.begin;
import genex.iterators.distance;
import genex.iterators.end;
import genex.iterators.next;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S>
auto do_intersperse(I &&first, S &&last, iter_value_t<I> const &separator) -> genex::generator<iter_value_t<I>> {
    if (genex::iterators::distance(first, last) == 0) { co_return; }
    co_yield *first;
    for (; first != last; genex::iterators::advance(first)) {
        co_yield separator;
        co_yield *first;
    }
}


template <range Rng>
auto do_intersperse(Rng &&rng, range_value_t<Rng> const &separator) -> genex::generator<range_value_t<Rng>> {
    if (genex::iterators::distance(genex::iterators::begin(rng), genex::iterators::end(rng)) == 0) { co_return; }
    co_yield *genex::iterators::begin(rng);
    for (auto it = genex::iterators::next(genex::iterators::begin(rng)); it != genex::iterators::end(rng); it = genex::iterators::next(it)) {
        co_yield separator;
        co_yield *it;
    }
}


template <iterator I, sentinel S, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, iter_value_t<I>>)
auto do_intersperse_with(I &&first, S &&last, F &&separator) -> genex::generator<iter_value_t<I>> {
    if (genex::iterators::distance(first, last) == 0) { co_return; }
    co_yield *first;
    for (; first != last; genex::iterators::advance(first)) {
        co_yield std::invoke(std::forward<F>(separator));
        co_yield *first;
    }
}


template <range Rng, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, range_value_t<Rng>>)
auto do_intersperse_with(Rng &&rng, F &&separator) -> genex::generator<range_value_t<Rng>> {
    if (genex::iterators::distance(genex::iterators::begin(rng), genex::iterators::end(rng)) == 0) { co_return; }
    co_yield *genex::iterators::begin(rng);
    for (auto it = genex::iterators::next(genex::iterators::begin(rng)); it != genex::iterators::end(rng); it = genex::iterators::next(it)) {
        co_yield std::invoke(std::forward<F>(separator));
        co_yield *it;
    }
}


namespace genex::views {
    struct intersperse_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last, iter_value_t<I> const &sep) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_intersperse, first, last, sep);
        }

        template <range Rng>
        auto operator()(Rng &&rng, range_value_t<Rng> const &sep) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_intersperse, rng, sep);
        }

        template <typename E>
        auto operator()(E &&sep) const -> decltype(auto) {
            MAP_TO_BASE(sep);
        }
    };

    struct intersperse_with_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, iter_value_t<I>>)
        auto operator()(I &&first, S &&last, F &&sep) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_intersperse_with, first, last, sep);
        }

        template <range Rng, std::invocable F> requires (std::same_as<std::invoke_result_t<F>, range_value_t<Rng>>)
        auto operator()(Rng &&rng, F &&sep) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_intersperse_with, rng, sep);
        }

        template <std::invocable F>
        auto operator()(F &&sep) const -> decltype(auto) {
            MAP_TO_BASE(sep);
        }
    };

    EXPORT_GENEX_STRUCT(intersperse);
    EXPORT_GENEX_STRUCT(intersperse_with);
}
