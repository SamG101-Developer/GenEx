#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/type_traits.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::invocable<iter_value_t<I>, iter_value_t<I>> BinaryOp>
auto do_accumulate(I &&first, S &&last, iter_value_t<I> const &init, BinaryOp &&binary_op, Proj &&proj = {}) -> iter_value_t<I> {
    auto acc = init;
    for (; first != last; ++first) {
        acc = std::invoke(std::forward<BinaryOp>(binary_op), std::move(acc), std::invoke(std::forward<Proj>(proj), *first));
    }
    return acc;
}


template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::invocable<range_value_t<Rng>, range_value_t<Rng>> BinaryOp>
auto do_accumulate(Rng &&rng, range_value_t<Rng> const &init, BinaryOp &&binary_op, Proj &&proj = {}) -> range_value_t<Rng> {
    auto acc = init;
    for (auto &&x : rng) {
        acc = std::invoke(std::forward<BinaryOp>(binary_op), std::move(acc), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)));
    }
    return acc;
}


namespace genex::algorithms {
    struct accumulate_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::invocable<iter_value_t<I>, iter_value_t<I>> BinaryOp>
        auto operator()(I &&first, S &&last, iter_value_t<I> const &init, BinaryOp &&binary_op, Proj &&proj = {}) const -> iter_value_t<I> {
            MAP_TO_IMPL(do_accumulate, first, last, init, binary_op, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::invocable<range_value_t<Rng>, range_value_t<Rng>> BinaryOp>
        auto operator()(Rng &&rng, range_value_t<Rng> const &init, BinaryOp &&binary_op, Proj &&proj = {}) const -> range_value_t<Rng> {
            MAP_TO_IMPL(do_accumulate, rng, init, binary_op, proj);
        }

        template <typename T, typename BinaryOp, typename Proj = meta::identity>
        auto operator()(T &&init, BinaryOp &&binary_op, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(init, binary_op, proj);
        }
    };

    EXPORT_GENEX_STRUCT(accumulate);
}
