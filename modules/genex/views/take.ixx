module;
#include <coroutine>
#include <functional>
#include <genex/macros.hpp>

export module genex.views.take;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.distance;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S>
auto do_take(I &&first, S &&last, const size_t n) -> genex::generator<iter_value_t<I>> {
    auto i = 0;
    for (; first != last; ++first) {
        if (i++ >= n) { break; }
        co_yield *first;
    }
}

template <range Rng>
auto do_take(Rng &&rng, const size_t n) -> genex::generator<range_value_t<Rng>> {
    auto i = 0;
    for (auto &&x : rng) {
        if (i++ >= n) { break; }
        co_yield std::forward<decltype(x)>(x);
    }
}

template <iterator I, sentinel S>
auto do_take_last(I &&first, S &&last, size_t n) -> genex::generator<iter_value_t<I>> {
    const auto length = genex::iterators::distance(first, last);
    auto i = 0;
    for (; first != last; ++first) {
        if (i++ < length - n) { continue; }
        co_yield *first;
    }
}

template <range Rng>
auto do_take_last(Rng &&rng, size_t n) -> genex::generator<range_value_t<Rng>> {
    const auto length = genex::iterators::distance(genex::iterators::begin(rng), genex::iterators::end(rng));
    auto i = 0;
    for (auto &&x : rng) {
        if (i++ < length - n) { continue; }
        co_yield std::forward<decltype(x)>(x);
    }
}

template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_take_while(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { break; }
        co_yield *first;
    }
}

template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_take_while(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { break; }
        co_yield std::forward<decltype(x)>(x);
    }
}

template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_take_until(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { break; }
        co_yield *first;
    }
}

template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_take_until(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { break; }
        co_yield std::forward<decltype(x)>(x);
    }
}

namespace genex::views {
    struct take_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last, const size_t n) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_take, first, last, n);
        }

        template <range Rng>
        auto operator()(Rng &&rng, const size_t n) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_take, rng, n);
        }

        auto operator()(size_t n) const -> decltype(auto) {
            MAP_TO_BASE(n);
        }
    };

    struct take_last_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last, size_t n) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_take_last, first, last, n);
        }

        template <range Rng>
        auto operator()(Rng &&rng, size_t n) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_take_last, rng, n);
        }

        auto operator()(size_t n) const -> decltype(auto) {
            MAP_TO_BASE(n);
        }
    };

    struct take_while_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_take_while, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_take_while, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };

    struct take_until_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_take_until, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_take_until, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };

    EXPORT_GENEX_STRUCT(take);
    EXPORT_GENEX_STRUCT(take_last);
    EXPORT_GENEX_STRUCT(take_while);
    EXPORT_GENEX_STRUCT(take_until);
}
