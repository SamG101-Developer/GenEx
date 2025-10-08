#pragma once
#include <functional>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename F>
    concept left_foldable_first_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>> and
        std::invocable<F, iter_value_t<I>, iter_reference_t<I>> and
        std::convertible_to<std::invoke_result_t<F, iter_value_t<I>, iter_reference_t<I>>, iter_value_t<I>>;

    template <typename Rng, typename F>
    concept left_foldable_first_range =
        forward_range<Rng> and
        left_foldable_first_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::algorithms {
    struct fold_left_first_fn {
        template <typename I, typename S, typename F>
        requires detail::concepts::left_foldable_first_iters<I, S, F>
        GENEX_INLINE constexpr auto operator()(I first, S last, F f) const {
            auto acc = *first;
            ++first;
            for (; first != last; ++first) {
                acc = std::invoke(f, std::move(acc), *first);
            }
            return acc;
        }

        template <typename Rng, typename F>
        requires detail::concepts::left_foldable_first_range<Rng, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f) const {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::move(f));
        }
    };

    inline constexpr fold_left_first_fn fold_left_first{};
}
