#pragma once
#include <functional>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename F>
    concept right_foldable_first_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>> and
        std::invocable<F, iter_value_t<I>, iter_reference_t<I>> and
        std::convertible_to<std::invoke_result_t<F, iter_value_t<I>, iter_reference_t<I>>, iter_value_t<I>>;

    template <typename Rng, typename F>
    concept right_foldable_first_range =
        bidirectional_range<Rng> and
        right_foldable_first_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::algorithms {
    struct fold_right_first_fn {
        template <typename I, typename S, typename F>
            requires concepts::right_foldable_first_iters<I, S, F>
        constexpr auto operator()(I first, S last, F &&f) const -> auto {
            auto acc = *--last;
            while (first != last) {
                --last;
                acc = std::invoke(std::forward<F>(f), *last, std::move(acc));
            }
            return acc;
        }

        template <typename Rng, typename F>
            requires concepts::right_foldable_first_range<Rng, F>
        constexpr auto operator()(Rng &&rng, F &&f) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<F>(f));
        }
    };

    EXPORT_GENEX_STRUCT(fold_right_first);
}
