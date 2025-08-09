#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S>
auto do_remove(I &&first, S &&last, iter_value_t<I> const &elem) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        if (*first == elem) { continue; }
        co_yield *first;
    }
}


template <range Rng>
auto do_remove(Rng &&rng, range_value_t<Rng> const &elem) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        if (x == elem) { continue; }
        co_yield std::forward<decltype(x)>(x);
    }
}


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_remove_if(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
        co_yield *first;
    }
}


template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_remove_if(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { continue; }
        co_yield std::forward<decltype(x)>(x);
    }
}


namespace genex::views {
    struct remove_fn final : detail::view_base {
        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, iter_value_t<I> const &elem) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_remove, first, last, elem);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> const &elem) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_remove, rng, elem);
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const -> decltype(auto) {
            MAP_TO_BASE(elem);
        }
    };

    struct remove_if_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_remove_if, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_remove_if, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };


    EXPORT_GENEX_STRUCT(remove);
    EXPORT_GENEX_STRUCT(remove_if);
}
