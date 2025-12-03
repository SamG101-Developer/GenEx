module;
#include <genex/macros.hpp>

export module genex.algorithms.fold_right;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;



namespace genex::algorithms::detail::concepts {
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


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename E, typename F>
    requires concepts::right_foldable_iters<I, S, E, F>
    GENEX_INLINE constexpr auto do_fold_right(I first, S last, E &&init, F &&f) -> E {
        auto &&acc = std::forward<E>(init);
        while (first != last) {
            --last;
            acc = meta::invoke(f, *last, std::move(acc));
        }
        return acc;
    }
}


namespace genex {
    struct fold_right_fn {
        template <typename I, typename S, typename E, typename F>
        requires algorithms::detail::concepts::right_foldable_iters<I, S, E, F>
        GENEX_INLINE constexpr auto operator()(I first, S last, E &&init, F &&f) const {
            return algorithms::detail::impl::do_fold_right(std::move(first), std::move(last), std::forward<E>(init), std::forward<F>(f));
        }

        template <typename Rng, typename E, typename F>
        requires algorithms::detail::concepts::fight_foldable_range<Rng, E, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E &&init, F &&f) const {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_fold_right(std::move(first), std::move(last), std::forward<E>(init), std::forward<F>(f));
        }
    };

    export inline constexpr fold_right_fn fold_right{};
}
