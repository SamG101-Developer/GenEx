#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/views/reverse.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel_for<I> S, typename E, std::invocable<E, iter_value_t<I>> F>
        requires (categories::input_iterator<I>)
    auto do_fold_left(I &&first, S &&last, E &&init, F &&f) -> E {
        auto &&acc = std::forward<E>(init);
        for (; first != last; ++first) {
            acc = std::invoke(std::forward<F>(f), std::move(acc), *first);
        }
        return acc;
    }

    template <range Rng, typename E, std::invocable<E, range_value_t<Rng>> F>
        requires (categories::input_range<Rng>)
    auto do_fold_left(Rng &&rng, E &&init, F &&f) -> E {
        auto &&acc = std::forward<E>(init);
        for (auto &&x : rng) {
            acc = std::invoke(std::forward<F>(f), std::move(acc), std::forward<decltype(x)>(x));
        }
        return acc;
    }

    template <iterator I, sentinel_for<I> S, typename E, std::invocable<E, iter_value_t<I>> F>
        requires (categories::bidirectional_iterator<I>)
    auto do_fold_right(I &&first, S &&last, E &&init, F &&f) -> E {
        auto &&acc = std::forward<E>(init);
        for (; first != last; --last) {
            acc = std::invoke(std::forward<F>(f), *last, std::move(acc));
        }
        return acc;
    }

    template <range Rng, typename E, std::invocable<E, range_value_t<Rng>> F>
        requires (categories::bidirectional_range<Rng>)
    auto do_fold_right(Rng &&rng, E &&init, F &&f) -> E {
        auto &&acc = std::forward<E>(init);
        for (auto &&x: rng | genex::views::reverse) {
            acc = std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x), std::move(acc));
        }
        return acc;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(fold_left) {
        template <iterator I, sentinel_for<I> S, typename E, std::invocable<E, iter_value_t<I>> F>
        requires (categories::input_iterator<I>)
        constexpr auto operator()(I &&first, S &&last, E &&init, F &&f) const -> auto {
            FWD_TO_IMPL(detail::do_fold_left, first, last, init, f);
        }

        template <range Rng, typename E, std::invocable<E, range_value_t<Rng>> F>
        requires (categories::input_range<Rng>)
        constexpr auto operator()(Rng &&rng, E &&init, F &&f) const -> auto {
            FWD_TO_IMPL(detail::do_fold_left, rng, init, f);
        }

        template <typename E, typename F>
        constexpr auto operator()(E &&init, F &&f) const -> auto {
            MAKE_CLOSURE(init, f);
        }
    };

    DEFINE_ALGORITHM(fold_right) {
        template <iterator I, sentinel_for<I> S, typename E, std::invocable<E, iter_value_t<I>> F>
        requires (categories::bidirectional_iterator<I>)
        constexpr auto operator()(I &&first, S &&last, E &&init, F &&f) const -> auto {
            FWD_TO_IMPL(detail::do_fold_right, first, last, init, f);
        }

        template <range Rng, typename E, std::invocable<E, range_value_t<Rng>> F>
        requires (categories::bidirectional_range<Rng>)
        constexpr auto operator()(Rng &&rng, E &&init, F &&f) const -> auto {
            FWD_TO_IMPL(detail::do_fold_right, rng, init, f);
        }

        template <typename E, typename F>
        constexpr auto operator()(E &&init, F &&f) const -> auto {
            MAKE_CLOSURE(init, f);
        }
    };

    EXPORT_GENEX_ALGORITHM(fold_left);
    EXPORT_GENEX_ALGORITHM(fold_right);
}
