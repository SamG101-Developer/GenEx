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
    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity>
    auto do_join(I &&first, S &&last, Proj &&proj) -> generator<char> {
        auto result = std::string();
        for (; first != last; ++first) {
            result += std::invoke(std::forward<Proj>(proj), *first);
        }
        for (auto &&c : result) { co_yield c; }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
    auto do_join(Rng &&rng, Proj &&proj) -> generator<char> {
        auto result = std::string();
        for (auto &&x : rng) {
            result += std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x));
        }
        for (auto &&c : result) { co_yield c; }
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity>
    auto do_join_with(I &&first, S &&last, std::invoke_result_t<Proj, iter_value_t<I>> &&sep, Proj &&proj = {}) -> generator<char> {
        auto result = std::string();
        for (; first != last; ++first) {
            if (!result.empty()) { result += sep; }
            result += std::invoke(std::forward<Proj>(proj), *first);
        }
        for (auto &&c : result) { co_yield c; }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
    auto do_join_with(Rng &&rng, std::invoke_result_t<Proj, range_value_t<Rng>> &&sep, Proj &&proj = {}) -> generator<char> {
        auto result = std::string();
        for (auto &&x : rng) {
            if (!result.empty()) { result += sep; }
            result += std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x));
        }
        for (auto &&c : result) { co_yield c; }
    }
}


namespace genex::views {
    DEFINE_VIEW(join) {
        DEFINE_OUTPUT_TYPE(join);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity>
        constexpr auto operator()(I &&first, S &&last, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join, first, last, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join, rng, proj);
        }

        template <typename T, std::invocable<T> Proj>
        constexpr auto operator()(Proj &&proj) const -> auto {
            MAKE_CLOSURE(proj);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    DEFINE_VIEW(join_with) {
        DEFINE_OUTPUT_TYPE(join_with);

        template <iterator I, sentinel S, typename T, std::invocable<iter_value_t<I>> Proj = meta::identity>
        constexpr auto operator()(I &&first, S &&last, T &&sep, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join_with, first, last, std::forward<T>(sep), proj);
        }

        template <range Rng, typename T, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, T &&sep, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_join_with, rng, std::forward<T>(sep), proj);
        }

        template <typename T, typename Proj = meta::identity>
        constexpr auto operator()(T &&sep, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(sep, proj);
        }
    };

    EXPORT_GENEX_VIEW(join);
    EXPORT_GENEX_VIEW(join_with);
}
