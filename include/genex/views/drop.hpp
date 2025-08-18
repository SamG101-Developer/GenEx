#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/meta.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_drop(I &&first, S &&last, const size_t n) -> generator<iter_value_t<I>> {
        auto i = 0;
        for (; first != last; ++first) {
            if (i++ < n) { continue; }
            co_yield *first;
        }
    }

    template <range Rng>
    auto do_drop(Rng &&rng, const size_t n) -> generator<range_value_t<Rng>> {
        auto i = 0;
        for (auto &&x : rng) {
            if (i++ < n) { continue; }
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <iterator I, sentinel S>
    auto do_drop_last(I &&first, S &&last, size_t n) -> generator<iter_value_t<I>> {
        const auto length = iterators::distance(first, last);
        size_t i = 0;
        for (; first != last; ++first) {
            if (i++ < length - n) { co_yield *first; }
            else { break; }
        }
    }

    template <range Rng>
    auto do_drop_last(Rng &&rng, size_t n) -> generator<range_value_t<Rng>> {
        const auto length = iterators::distance(iterators::begin(rng), iterators::end(rng));
        size_t i = 0;
        for (auto &&x : rng) {
            if (i++ < length - n) { co_yield std::forward<decltype(x)>(x); }
            else { break; }
        }
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_drop_while(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        auto dropping = true;
        for (; first != last; ++first) {
            if (dropping && std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
            dropping = false;
            co_yield *first;
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_drop_while(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        auto dropping = true;
        for (auto &&x : rng) {
            if (dropping && std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { continue; }
            dropping = false;
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_drop_until(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        auto dropping = true;
        for (; first != last; ++first) {
            if (dropping && !std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
            dropping = false;
            co_yield *first;
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_drop_until(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        auto dropping = true;
        for (auto &&x : rng) {
            if (dropping && !std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { continue; }
            dropping = false;
            co_yield std::forward<decltype(x)>(x);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(drop) {
        DEFINE_OUTPUT_TYPE(drop);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, const size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop, first, last, n);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, const size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop, rng, n);
        }

        constexpr auto operator()(size_t n) const -> auto {
            MAKE_CLOSURE(n);
        }
    };

    DEFINE_VIEW(drop_last) {
        DEFINE_OUTPUT_TYPE(drop_last);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop_last, first, last, n);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, size_t n) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop_last, rng, n);
        }

        constexpr auto operator()(size_t n) const -> auto {
            MAKE_CLOSURE(n);
        }
    };

    DEFINE_VIEW(drop_while) {
        DEFINE_OUTPUT_TYPE(drop_while);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop_while, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop_while, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(pred, proj);
        }
    };

    DEFINE_VIEW(drop_until) {
        DEFINE_OUTPUT_TYPE(drop_until);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop_until, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_drop_until, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(pred, proj);
        }
    };

    EXPORT_GENEX_VIEW(drop);
    EXPORT_GENEX_VIEW(drop_last);
    EXPORT_GENEX_VIEW(drop_while);
    EXPORT_GENEX_VIEW(drop_until);
}
