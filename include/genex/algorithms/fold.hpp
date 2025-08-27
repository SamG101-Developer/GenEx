#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/views/drop.hpp>
#include <genex/views/reverse.hpp>


namespace genex::algorithms {
    template <typename I, typename S, typename E, typename F>
    concept can_fold_left_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<F, E, iter_value_t<I>> and
        std::convertible_to<std::invoke_result_t<F, E, iter_value_t<I>>, E> and
        std::movable<E>;

    template <typename Rng, typename E, typename F>
    concept can_fold_left_range =
        input_range<Rng> and
        std::invocable<F, E, range_value_t<Rng>> and
        std::convertible_to<std::invoke_result_t<F, E, range_value_t<Rng>>, E> and
        std::movable<E>;

    template <typename I, typename S, typename F>
    concept can_fold_left_first_iters =
        forward_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<F, iter_value_t<I>, iter_value_t<I>> and
        requires(F &f, iter_value_t<I> a, iter_value_t<I> b) { { f(a, b) } -> std::movable; };

    template <typename Rng, typename F>
    concept can_fold_left_first_range =
        forward_range<Rng> and
        std::invocable<F, range_value_t<Rng>, range_value_t<Rng>> and
        requires(F &f, range_value_t<Rng> a, range_value_t<Rng> b) { { f(a, b) } -> std::movable; };

    template <typename I, typename S, typename E, typename F>
    concept can_fold_right_iters =
        bidirectional_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<F, iter_value_t<I>, E> and
        std::convertible_to<std::invoke_result_t<F, iter_value_t<I>, E>, E> and
        std::movable<E>;

    template <typename Rng, typename E, typename F>
    concept can_fold_right_range =
        bidirectional_range<Rng> and
        std::invocable<F, range_value_t<Rng>, E> and
        std::convertible_to<std::invoke_result_t<F, range_value_t<Rng>, E>, E> and
        std::movable<E>;

    template <typename I, typename S, typename F>
    concept can_fold_right_first_iters =
        bidirectional_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<F, iter_value_t<I>, iter_value_t<I>> and
        requires(F &f, iter_value_t<I> a, iter_value_t<I> b) { { f(a, b) } -> std::movable; };

    template <typename Rng, typename F>
    concept can_fold_right_first_range =
        bidirectional_range<Rng> and
        std::invocable<F, range_value_t<Rng>, range_value_t<Rng>> and
        requires(F &f, range_value_t<Rng> a, range_value_t<Rng> b) { { f(a, b) } -> std::movable; };

    DEFINE_ALGORITHM(fold_left) {
        template <typename I, typename S, typename E, typename F> requires can_fold_left_iters<I, S, E, F>
        constexpr auto operator()(I first, S last, E &&init, F &&f) const -> auto {
            auto &&acc = std::forward<E>(init);
            for (; first != last; ++first) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), *first);
            }
            return acc;
        }

        template <typename Rng, typename E, typename F> requires can_fold_left_range<Rng, E, F>
        constexpr auto operator()(Rng &&rng, E &&init, F &&f) const -> auto {
            auto &&acc = std::forward<E>(init);
            for (auto &&x : rng) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), std::forward<decltype(x)>(x));
            }
            return acc;
        }

        template <typename E, typename F> requires (not input_range<std::remove_cvref_t<E>>)
        constexpr auto operator()(E &&init, F &&f) const -> auto {
            return
                [FWD_CAPTURES(init, f)]<typename Rng> requires can_fold_left_range<Rng, E, F>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(init), std::forward<F>(f));
            };
        }
    };

    DEFINE_ALGORITHM(fold_left_first) {
        template <typename I, typename S, typename F> requires can_fold_left_first_iters<I, S, F>
        constexpr auto operator()(I first, S last, F &&f) const -> auto {
            auto acc = *first++;
            for (; first != last; ++first) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), *first);
            }
            return acc;
        }

        template <typename Rng, typename F> requires can_fold_left_first_range<Rng, F>
        constexpr auto operator()(Rng &&rng, F &&f) const -> auto {
            auto acc = *iterators::begin(rng);
            for (auto &&x : rng | views::drop(1)) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), std::forward<decltype(x)>(x));
            }
            return acc;
        }

        template <typename F> requires (not input_range<std::remove_cvref_t<F>>)
        constexpr auto operator()(F &&f) const -> auto {
            return
                [FWD_CAPTURES(f)]<typename Rng> requires can_fold_left_first_range<Rng, F>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(f));
            };
        }
    };

    DEFINE_ALGORITHM(fold_right) {
        template <typename I, typename S, typename E, typename F> requires can_fold_right_iters<I, S, E, F>
        constexpr auto operator()(I first, S last, E &&init, F &&f) const -> auto {
            auto &&acc = std::forward<E>(init);
            for (; first != last; --last) {
                acc = std::invoke(std::forward<F>(f), *last, std::move(acc));
            }
            return acc;
        }

        template <typename Rng, typename E, typename F> requires can_fold_right_range<Rng, E, F>
        constexpr auto operator()(Rng &&rng, E &&init, F &&f) const -> auto {
            auto &&acc = std::forward<E>(init);
            for (auto &&x : rng | views::reverse) {
                acc = std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x), std::move(acc));
            }
            return acc;
        }

        template <typename E, typename F> requires (not input_range<std::remove_cvref_t<E>>)
        constexpr auto operator()(E &&init, F &&f) const -> auto {
            return
                [FWD_CAPTURES(init, f)]<typename Rng> requires can_fold_right_range<Rng, E, F>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(init), std::forward<F>(f));
            };
        }
    };

    DEFINE_ALGORITHM(fold_right_first) {
        template <typename I, typename S, typename F> requires can_fold_right_first_iters<I, S, F>
        constexpr auto operator()(I first, S last, F &&f) const -> auto {
            auto acc = *--last;
            for (; first != last; --last) {
                acc = std::invoke(std::forward<F>(f), *--last, std::move(acc));
            }
            return acc;
        }

        template <typename Rng, typename F> requires can_fold_right_first_range<Rng, F>
        constexpr auto operator()(Rng &&rng, F &&f) const -> auto {
            auto acc = *--iterators::end(rng);
            for (auto &&x : rng | views::reverse | views::drop(1)) {
                acc = std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x), std::move(acc));
            }
            return acc;
        }

        template <typename F> requires (not input_range<std::remove_cvref_t<F>>)
        constexpr auto operator()(F &&f) const -> auto {
            return
                [FWD_CAPTURES(f)]<typename Rng> requires can_fold_right_first_range<Rng, F>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(f));
            };
        }
    };

    EXPORT_GENEX_ALGORITHM(fold_left);
    EXPORT_GENEX_ALGORITHM(fold_left_first);
    EXPORT_GENEX_ALGORITHM(fold_right);
    EXPORT_GENEX_ALGORITHM(fold_right_first);
}
