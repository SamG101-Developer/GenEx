#pragma once
#include <functional>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename F>
    concept right_foldable_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<E> and
        std::invocable<F, iter_reference_t<I>, E> and
        std::convertible_to<std::invoke_result_t<F, iter_reference_t<I>, E>, E>;

    template <typename Rng, typename E, typename F>
    concept fight_foldable_range =
        bidirectional_range<Rng> and
        right_foldable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, F>;
}


namespace genex::algorithms {
    struct fold_right_fn {
        template <typename I, typename S, typename E, typename F>
            requires concepts::right_foldable_iters<I, S, E, F>
        constexpr auto operator()(I first, S last, E &&init, F &&f) const -> auto {
            auto &&acc = std::forward<E>(init);
            while (first != last) {
                --last;
                acc = std::invoke(std::forward<F>(f), *last, std::move(acc));
            }
            return acc;
        }

        template <typename Rng, typename E, typename F>
            requires concepts::fight_foldable_range<Rng, E, F>
        constexpr auto operator()(Rng &&rng, E &&init, F &&f) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<E>(init), std::forward<F>(f));
        }
    };

    EXPORT_GENEX_STRUCT(fold_right);
}
