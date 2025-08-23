#pragma once
#include <functional>
#include <genex/actions/push.hpp>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <range Rng> requires (
        categories::input_range<Rng>)
    auto do_copy(Rng &&rng) -> std::remove_cvref_t<Rng> {
        auto copy = std::remove_cvref_t<Rng>();
        auto it = iterators::begin(rng);
        for (; it != iterators::end(rng); ++it) {
            copy |= actions::push_back(*it);
        }
        return std::move(copy);
    }

    template <range Rng> requires (
        categories::input_range<Rng>)
    auto copy_n(Rng &&rng, const std::size_t n) -> std::remove_cvref_t<Rng> {
        auto copy = std::remove_cvref_t<Rng>();
        auto it = iterators::begin(rng);
        for (std::size_t i = 0; i < n && it != iterators::end(rng); ++i, ++it) {
            copy |= actions::push_back(*it);
        }
        return std::move(copy);
    }

    template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old> and
        std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
    auto copy_if(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> std::remove_cvref_t<Rng> {
        auto copy = std::remove_cvref_t<Rng>();
        auto it = iterators::begin(rng);
        for (; it != iterators::end(rng); ++it) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *it))) {
                copy |= actions::push_back(*it);
            }
        }
        return std::move(copy);
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(copy) {
        template <range Rng> requires (
            categories::input_range<Rng>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL(detail::do_copy, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    DEFINE_ALGORITHM(copy_n) {
        template <range Rng> requires (
            categories::input_range<Rng>)
        constexpr auto operator()(Rng &&rng, const std::size_t n) const -> auto {
            FWD_TO_IMPL(detail::copy_n, rng, n);
        }

        constexpr auto operator()(const std::size_t n) const -> auto {
            MAKE_CLOSURE(n);
        }
    };

    DEFINE_ALGORITHM(copy_if) {
        template <range Rng, typename Old = range_value_t<Rng>, std::invocable<Old> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, Old>> Pred> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old> and
            std::convertible_to<std::invoke_result_t<Proj, Old>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::copy_if, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(pred, proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(copy);
    EXPORT_GENEX_ALGORITHM(copy_n);
    EXPORT_GENEX_ALGORITHM(copy_if);
}
