#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/views/drop.hpp>
#include <genex/views/reverse.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename F>
    concept can_fold_left_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::movable<E> and
        std::invocable<F, E, iter_reference_t<I>> and
        std::convertible_to<std::invoke_result_t<F, E, iter_reference_t<I>>, E>;

    template <typename I, typename S, typename F>
    concept can_fold_left_first_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::movable<iter_value_t<I>> and
        std::invocable<F, iter_value_t<I>, iter_reference_t<I>> and
        std::convertible_to<std::invoke_result_t<F, iter_value_t<I>, iter_reference_t<I>>, iter_value_t<I>>;

    template <typename I, typename S, typename E, typename F>
    concept can_fold_right_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::movable<E> and
        std::invocable<F, iter_reference_t<I>, E> and
        std::convertible_to<std::invoke_result_t<F, iter_reference_t<I>, E>, E>;

    template <typename I, typename S, typename F>
    concept can_fold_right_first_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::movable<iter_value_t<I>> and
        std::invocable<F, iter_value_t<I>, iter_reference_t<I>> and
        std::convertible_to<std::invoke_result_t<F, iter_value_t<I>, iter_reference_t<I>>, iter_value_t<I>>;

    template <typename Rng, typename E, typename F>
    concept can_fold_left_range =
        input_range<Rng> and
        can_fold_left_iters<iterator_t<Rng>, sentinel_t<Rng>, E, F>;

    template <typename Rng, typename F>
    concept can_fold_left_first_range =
        forward_range<Rng> and
        can_fold_left_first_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;

    template <typename Rng, typename E, typename F>
    concept can_fold_right_range =
        bidirectional_range<Rng> and
        can_fold_right_iters<iterator_t<Rng>, sentinel_t<Rng>, E, F>;

    template <typename Rng, typename F>
    concept can_fold_right_first_range =
        bidirectional_range<Rng> and
        can_fold_right_first_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(fold_left) {
        template <typename I, typename S, typename E, typename F> requires concepts::can_fold_left_iters<I, S, E, F>
        auto operator()(I first, S last, E &&init, F &&f) const -> auto {
            auto &&acc = std::forward<E>(init);
            for (; first != last; ++first) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), *first);
            }
            return acc;
        }

        template <typename Rng, typename E, typename F> requires concepts::can_fold_left_range<Rng, E, F>
        auto operator()(Rng &&rng, E &&init, F &&f) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<E>(init), std::forward<F>(f));
        }
    };

    DEFINE_ALGORITHM(fold_left_first) {
        template <typename I, typename S, typename F> requires concepts::can_fold_left_first_iters<I, S, F>
        auto operator()(I first, S last, F &&f) const -> auto {
            auto acc = *first;
            ++first;
            for (; first != last; ++first) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), *first);
            }
            return acc;
        }

        template <typename Rng, typename F> requires concepts::can_fold_left_first_range<Rng, F>
        auto operator()(Rng &&rng, F &&f) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<F>(f));
        }
    };

    DEFINE_ALGORITHM(fold_right) {
        template <typename I, typename S, typename E, typename F> requires concepts::can_fold_right_iters<I, S, E, F>
        auto operator()(I first, S last, E &&init, F &&f) const -> auto {
            auto &&acc = std::forward<E>(init);
            while (first != last) {
                --last;
                acc = std::invoke(std::forward<F>(f), *last, std::move(acc));
            }
            return acc;
        }

        template <typename Rng, typename E, typename F> requires concepts::can_fold_right_range<Rng, E, F>
        auto operator()(Rng &&rng, E &&init, F &&f) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<E>(init), std::forward<F>(f));
        }
    };

    DEFINE_ALGORITHM(fold_right_first) {
        template <typename I, typename S, typename F> requires concepts::can_fold_right_first_iters<I, S, F>
        auto operator()(I first, S last, F &&f) const -> auto {
            auto acc = *--last;
            while (first != last) {
                --last;
                acc = std::invoke(std::forward<F>(f), *last, std::move(acc));
            }
            return acc;
        }

        template <typename Rng, typename F> requires concepts::can_fold_right_first_range<Rng, F>
        auto operator()(Rng &&rng, F &&f) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<F>(f));
        }
    };

    EXPORT_GENEX_ALGORITHM(fold_left);
    EXPORT_GENEX_ALGORITHM(fold_left_first);
    EXPORT_GENEX_ALGORITHM(fold_right);
    EXPORT_GENEX_ALGORITHM(fold_right_first);
}
