#pragma once
#include <coroutine>
#include <functional>
#include <genex/categories.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_take(I &&first, S &&last, const size_t n) -> generator<iter_value_t<I>> {
        auto i = 0;
        for (; first != last; ++first) {
            if (i++ >= n) { break; }
            co_yield *first;
        }
    }

    template <range Rng>
    auto do_take(Rng &&rng, const size_t n) -> generator<range_value_t<Rng>> {
        auto i = 0;
        for (auto &&x : rng) {
            if (i++ >= n) { break; }
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <iterator I, sentinel S>
    auto do_take_last(I &&first, S &&last, size_t n) -> generator<iter_value_t<I>> {
        const auto length = iterators::distance(first, last);
        auto i = 0;
        for (; first != last; ++first) {
            if (i++ < length - n) { continue; }
            co_yield *first;
        }
    }

    template <range Rng>
    auto do_take_last(Rng &&rng, size_t n) -> generator<range_value_t<Rng>> {
        const auto length = iterators::distance(iterators::begin(rng), iterators::end(rng));
        auto i = 0;
        for (auto &&x : rng) {
            if (i++ < length - n) { continue; }
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_take_while(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { break; }
            co_yield *first;
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_take_while(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { break; }
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_take_until(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { break; }
            co_yield *first;
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_take_until(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { break; }
            co_yield std::forward<decltype(x)>(x);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(take) {
        DEFINE_OUTPUT_TYPE(take);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, const size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take, first, last, n);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, const size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take, rng, n);
        }

        constexpr auto operator()(size_t n) const -> auto {
            MAP_TO_BASE(n);
        }
    };

    DEFINE_VIEW(take_last) {
        DEFINE_OUTPUT_TYPE(take_last);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take_last, first, last, n);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take_last, rng, n);
        }

        constexpr auto operator()(size_t n) const -> auto {
            MAP_TO_BASE(n);
        }
    };

    DEFINE_VIEW(take_while) {
        DEFINE_OUTPUT_TYPE(take_while);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take_while, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take_while, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAP_TO_BASE(pred, proj);
        }
    };

    DEFINE_VIEW(take_until) {
        DEFINE_OUTPUT_TYPE(take_until);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take_until, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_take_until, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAP_TO_BASE(pred, proj);
        }
    };

    EXPORT_GENEX_VIEW(take);
    EXPORT_GENEX_VIEW(take_last);
    EXPORT_GENEX_VIEW(take_while);
    EXPORT_GENEX_VIEW(take_until);
}
