module;
#include <genex/macros.hpp>

export module genex.algorithms.fold_left;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;


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


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename E, typename F>
    requires concepts::left_foldable_iters<I, S, E, F>
    GENEX_INLINE constexpr auto do_fold_left(I first, S last, E &&init, F &&f) -> E {
        auto &&acc = std::forward<E>(init);
        for (; first != last; ++first) {
            acc = meta::invoke(f, std::move(acc), *first);
        }
        return acc;
    }
}


namespace genex {
    struct fold_left_fn {
        template <typename I, typename S, typename E, typename F>
        requires algorithms::detail::concepts::left_foldable_iters<I, S, E, F>
        GENEX_INLINE constexpr auto operator()(I first, S last, E &&init, F &&f) const {
            return algorithms::detail::impl::do_fold_left(std::move(first), std::move(last), std::forward<E>(init), std::forward<F>(f));
        }

        template <typename Rng, typename E, typename F>
        requires algorithms::detail::concepts::can_fold_left_range<Rng, E, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E &&init, F &&f) const {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_fold_left(std::move(first), std::move(last), std::forward<E>(init), std::forward<F>(f));
        }
    };


    export inline constexpr fold_left_fn fold_left{};
}
