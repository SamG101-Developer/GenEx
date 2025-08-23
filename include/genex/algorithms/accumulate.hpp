#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/type_traits.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel_for<I> S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::invocable<iter_value_t<I>, iter_value_t<I>> BinaryOp>
    auto do_accumulate(I &&first, S &&last, iter_value_t<I> const &init, BinaryOp &&binary_op, Proj &&proj = {}) -> iter_value_t<I> {
        auto acc = init;
        for (; first != last; ++first) {
            acc = std::invoke(std::forward<BinaryOp>(binary_op), std::move(acc), std::invoke(std::forward<Proj>(proj), *first));
        }
        return acc;
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::invocable<range_value_t<Rng>, range_value_t<Rng>> BinaryOp>
    auto do_accumulate(Rng &&rng, range_value_t<Rng> const &init, BinaryOp &&binary_op, Proj &&proj = {}) -> range_value_t<Rng> {
        auto acc = init;
        for (auto &&x : rng) {
            acc = std::invoke(std::forward<BinaryOp>(binary_op), std::move(acc), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)));
        }
        return acc;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(accumulate) {
        template <iterator I, sentinel_for<I> S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::invocable<iter_value_t<I>, iter_value_t<I>> BinaryOp>
        constexpr auto operator()(I &&first, S &&last, iter_value_t<I> const &init, BinaryOp &&binary_op, Proj &&proj = {}) const -> iter_value_t<I> {
            FWD_TO_IMPL(detail::do_accumulate, first, last, init, binary_op, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::invocable<range_value_t<Rng>, range_value_t<Rng>> BinaryOp>
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> const &init, BinaryOp &&binary_op, Proj &&proj = {}) const -> range_value_t<Rng> {
            FWD_TO_IMPL(detail::do_accumulate, rng, init, binary_op, proj);
        }

        template <typename T, typename BinaryOp, typename Proj = meta::identity>
        constexpr auto operator()(T &&init, BinaryOp &&binary_op, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(init, binary_op, proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(accumulate);
}
