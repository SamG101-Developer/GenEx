#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
        categories::input_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old>)
    auto do_any_of(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> bool {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                return true;
            }
        }
        return false;
    }

    template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old> and
        std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
    auto do_any_of(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> bool {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) {
                return true;
            }
        }
        return false;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(any_of) {
        template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old>)
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> bool {
            FWD_TO_IMPL(detail::do_any_of, first, last, pred, proj);
        }

        template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old> and
            std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> bool {
            FWD_TO_IMPL(detail::do_any_of, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(pred, proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(any_of);
}
