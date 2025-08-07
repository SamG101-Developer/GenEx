#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S, std::invocable<iter_value_t<I>, iter_value_t<I>> F>
auto do_fold_left(I &&first, S &&last, iter_value_t<I> &&init, F &&f) -> iter_value_t<I> {
    auto &&acc = std::forward<iter_value_t<I>>(init);
    for (; first != last; ++first) {
        acc = std::invoke(std::forward<F>(f), std::move(acc), *first);
    }
    return acc;
}


template <range Rng, std::invocable<range_value_t<Rng>, range_value_t<Rng>> F>
auto do_fold_left(Rng &&rng, range_value_t<Rng> &&init, F &&f) -> range_value_t<Rng> {
    auto &&acc = std::forward<range_value_t<Rng>>(init);
    for (auto &&x : rng) {
        acc = std::invoke(std::forward<F>(f), std::move(acc), std::forward<decltype(x)>(x));
    }
    return acc;
}


template <iterator I, sentinel S, std::invocable<iter_value_t<I>, iter_value_t<I>> F>
auto do_fold_right(I &&first, S &&last, iter_value_t<I> &&init, F &&f) -> iter_value_t<I> {
    auto &&acc = std::forward<iter_value_t<I>>(init);
    for (; first != last; --last) {
        acc = std::invoke(std::forward<F>(f), *last, std::move(acc));
    }
    return acc;
}


template <range Rng, std::invocable<range_value_t<Rng>, range_value_t<Rng>> F>
auto do_fold_right(Rng &&rng, range_value_t<Rng> &&init, F &&f) -> range_value_t<Rng> {
    auto &&acc = std::forward<range_value_t<Rng>>(init);
    for (auto it = genex::iterators::end(rng); it != genex::iterators::begin(rng); --it) {
        acc = std::invoke(std::forward<F>(f), *genex::iterators::next(it, -1), std::move(acc));
    }
    return acc;
}


namespace genex::views {
    struct fold_left_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>, iter_value_t<I>> F>
        auto operator()(I &&first, S &&last, iter_value_t<I> &&init, F &&f) const -> iter_value_t<I> {
            MAP_TO_IMPL(do_fold_left, first, last, init, f);
        }

        template <range Rng, std::invocable<range_value_t<Rng>, range_value_t<Rng>> F>
        auto operator()(Rng &&rng, range_value_t<Rng> &&init, F &&f) const -> range_value_t<Rng> {
            MAP_TO_IMPL(do_fold_left, rng, init, f);
        }

        template <typename E, std::invocable<E, E> F>
        auto operator()(E &&init, F &&f) const -> decltype(auto) {
            MAP_TO_BASE(init, f);
        }
    };

    struct fold_right_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>, iter_value_t<I>> F>
        auto operator()(I &&first, S &&last, iter_value_t<I> &&init, F &&f) const -> iter_value_t<I> {
            MAP_TO_IMPL(do_fold_right, first, last, init, f);
        }

        template <range Rng, std::invocable<range_value_t<Rng>, range_value_t<Rng>> F>
        auto operator()(Rng &&rng, range_value_t<Rng> &&init, F &&f) const -> range_value_t<Rng> {
            MAP_TO_IMPL(do_fold_right, rng, init, f);
        }

        template <typename E, std::invocable<E, E> F>
        auto operator()(E &&init, F &&f) const -> decltype(auto) {
            MAP_TO_BASE(init, f);
        }
    };

    EXPORT_GENEX_STRUCT(fold_left);
    EXPORT_GENEX_STRUCT(fold_right);
}
