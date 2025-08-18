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
    template <iterator I, sentinel S>
    auto do_remove(I &&first, S &&last, iter_value_t<I> const &elem) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (*first == elem) { continue; }
            co_yield *first;
        }
    }

    template <range Rng>
    auto do_remove(Rng &&rng, range_value_t<Rng> const &elem) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (x == elem) { continue; }
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_remove_if(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
            co_yield *first;
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_remove_if(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { continue; }
            co_yield std::forward<decltype(x)>(x);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(remove) {
        DEFINE_OUTPUT_TYPE(remove);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, iter_value_t<I> const &elem) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove, first, last, elem);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> const &elem) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove, rng, elem);
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const -> auto {
            MAP_TO_BASE(elem);
        }
    };

    DEFINE_VIEW(remove_if) {
        DEFINE_OUTPUT_TYPE(remove_if);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove_if, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_remove_if, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAP_TO_BASE(pred, proj);
        }
    };


    EXPORT_GENEX_VIEW(remove);
    EXPORT_GENEX_VIEW(remove_if);
}
