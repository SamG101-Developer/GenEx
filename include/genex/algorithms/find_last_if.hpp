#pragma once
#include <functional>
#include <genex/iterators/iter_pair.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_iters_optimized =
        std::bidirectional_iterator<I> and
        findable_last_if_iters<I, S, Pred, Proj>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_iters_unoptimized =
        not findable_last_if_iters_optimized<I, S, Pred, Proj> and
        findable_last_if_iters<I, S, Pred, Proj>;

    template <typename Rng, typename E, typename Proj>
    concept findable_last_if_range =
        input_range<Rng> and
        findable_last_if_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms {
    struct find_last_if_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
            requires concepts::findable_last_if_iters_optimized<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto result = last;
            for (; last != first; --last) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *std::prev(last)))) {
                    return std::prev(last);
                }
            }
            return result;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
            requires concepts::findable_last_if_iters_unoptimized<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto result = last;
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    result = first;
                }
            }
            return result;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
            requires concepts::findable_last_if_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(find_last_if);
}
