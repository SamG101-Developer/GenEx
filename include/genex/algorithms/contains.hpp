#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel_for<I> S, typename Old1, typename Old2 = iter_value_t<I>, std::invocable<Old2> Proj = meta::identity> requires (
        categories::input_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old1> and
        std::equality_comparable_with<iter_value_t<I>, Old2> and
        std::convertible_to<std::invoke_result_t<Proj, Old2>, iter_value_t<I>>)
    auto do_contains(I &&first, S &&last, Old1 &&elem, Proj &&proj = {}) -> bool {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Proj>(proj), *first) == elem) { return true; }
        }
        return false;
    }

    template <range Rng, typename Old1, typename Old2 = range_value_t<Rng>, std::invocable<Old2> Proj = meta::identity> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old1> and
        std::equality_comparable_with<range_value_t<Rng>, Old2> and
        std::convertible_to<std::invoke_result_t<Proj, Old2>, range_value_t<Rng>>)
    auto do_contains(Rng &&rng, Old1 &&elem, Proj &&proj = {}) -> bool {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == elem) { return true; }
        }
        return false;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(contains) {
        template <iterator I, sentinel_for<I> S, typename Old1, typename Old2 = iter_value_t<I>, std::invocable<Old2> Proj = meta::identity> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old1> and
            std::equality_comparable_with<iter_value_t<I>, Old2> and
            std::convertible_to<std::invoke_result_t<Proj, Old2>, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, Old1 &&elem, Proj &&proj = {}) const -> bool {
            FWD_TO_IMPL(detail::do_contains, first, last, elem, proj);
        }

        template <range Rng, typename Old1, typename Old2 = range_value_t<Rng>, std::invocable<Old2> Proj = meta::identity> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old1> and
            std::equality_comparable_with<range_value_t<Rng>, Old2> and
            std::convertible_to<std::invoke_result_t<Proj, Old2>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, Old1 &&elem, Proj &&proj = {}) const -> bool {
            FWD_TO_IMPL(detail::do_contains, rng, elem, proj);
        }

        template <typename Old1, std::invocable<Old1> Proj = meta::identity>
        constexpr auto operator()(Old1 &&elem, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(elem, proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(contains);
}
