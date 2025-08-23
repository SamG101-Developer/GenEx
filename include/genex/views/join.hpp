#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>> requires (
        categories::input_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old>)
    auto do_join(I &&first, S &&last) -> generator<char> {
        auto result = std::string();
        for (; first != last; ++first) {
            result += *first;
        }
        for (auto &&c : result) { co_yield c; }
    }

    template <range Rng, typename Old = range_value_t<Rng>> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old>)
    auto do_join(Rng &&rng) -> generator<char> {
        auto result = std::string();
        for (auto &&x : rng) {
            result += std::forward<decltype(x)>(x);
        }
        for (auto &&c : result) { co_yield c; }
    }

    template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, typename New> requires (
        categories::input_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old> and
        std::convertible_to<New, iter_value_t<I>>)
    auto do_join_with(I &&first, S &&last, New &&sep) -> generator<char> {
        auto result = std::string();
        for (; first != last; ++first) {
            if (!result.empty()) { result += sep; }
            result += *first;
        }
        for (auto &&c : result) { co_yield c; }
    }

    template <range Rng, typename New, typename Old = range_value_t<Rng>> requires (
        categories::input_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old> and
        std::convertible_to<New, range_value_t<Rng>>)
    auto do_join_with(Rng &&rng, New &&sep) -> generator<char> {
        auto result = std::string();
        for (auto &&x : rng) {
            if (!result.empty()) { result += sep; }
            result += std::forward<decltype(x)>(x);
        }
        for (auto &&c : result) { co_yield c; }
    }
}


namespace genex::views {
    DEFINE_VIEW(join) {
        DEFINE_OUTPUT_TYPE(join);

        template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old>)
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join, first, last);
        }

        template <range Rng, typename Old = range_value_t<Rng>> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join, rng);
        }

        template <typename Proj = meta::identity> requires (
            not range<Proj> and
            not iterator<Proj>)
        constexpr auto operator()(Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(proj);
        }
    };

    DEFINE_VIEW(join_with) {
        DEFINE_OUTPUT_TYPE(join_with);

        template <iterator I, sentinel_for<I> S, typename Old = iter_value_t<I>, typename New> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old> and
            std::convertible_to<New, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, New &&sep) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join_with, first, last, sep);
        }

        template <range Rng, typename New, typename Old = range_value_t<Rng>> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old> and
            std::convertible_to<New, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, New &&sep) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join_with, rng, sep);
        }

        template <typename New, typename Proj = meta::identity> requires (
            not range<New> and
            not iterator<New>)
        constexpr auto operator()(New &&sep, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(sep, proj);
        }
    };

    EXPORT_GENEX_VIEW(join);
    EXPORT_GENEX_VIEW(join_with);
}
