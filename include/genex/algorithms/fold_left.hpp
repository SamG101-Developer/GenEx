#pragma once
#include <functional>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename E, typename F>
    concept left_foldable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<E> and
        std::invocable<F, E, iter_reference_t<I>> and
        std::convertible_to<std::invoke_result_t<F, E, iter_reference_t<I>>, E>;

    template <typename Rng, typename E, typename F>
    concept can_fold_left_range =
        input_range<Rng> and
        left_foldable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, F>;
}


namespace genex::algorithms {
    struct fold_left_fn {
        template <typename I, typename S, typename E, typename F>
        requires detail::concepts::left_foldable_iters<I, S, E, F>
        GENEX_INLINE constexpr auto operator()(I first, S last, E init, F f) const {
            auto acc = init;
            for (; first != last; ++first) {
                acc = std::invoke(f, std::move(acc), *first);
            }
            return acc;
        }

        template <typename Rng, typename E, typename F>
        requires detail::concepts::can_fold_left_range<Rng, E, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E init, F f) const {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::move(init), std::move(f));
        }
    };


    inline constexpr fold_left_fn fold_left{};
}
