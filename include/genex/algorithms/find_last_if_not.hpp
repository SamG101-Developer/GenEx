#pragma once
#include <genex/algorithms/find_if.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_not_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_not_iters_optimized =
        std::bidirectional_iterator<I> and
        findable_last_if_not_iters<I, S, Pred, Proj>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_not_iters_unoptimized =
        not findable_last_if_not_iters_optimized<I, S, Pred, Proj> and
        findable_last_if_not_iters<I, S, Pred, Proj>;

    template <typename Rng, typename E, typename Proj>
    concept findable_last_if_not_range =
        input_range<Rng> and
        findable_last_if_not_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms {
    struct find_last_if_not_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
            requires concepts::findable_last_if_not_iters_unoptimized<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto found_last = last;
            for (; first != last; ++first) {
                if (not std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    found_last = first;
                }
            }
            return found_last;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
            requires concepts::findable_last_if_not_iters_optimized<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto true_last = last;
            for (; last != first; --last) {
                if (not std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *last))) {
                    return last;
                }
            }
            return true_last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
            requires concepts::findable_last_if_not_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(find_last_if_not);
}
