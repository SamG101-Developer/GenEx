#pragma once
#include <functional>
#include <genex/algorithms/find.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept findable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename I, typename S, typename E, typename Proj>
    concept findable_last_iters_optimized =
        std::bidirectional_iterator<I> and
        findable_last_iters<I, S, E, Proj>;

    template <typename I, typename S, typename E, typename Proj>
    concept findable_last_iters_unoptimized =
        not findable_last_iters_optimized<I, S, E, Proj> and
        findable_last_iters<I, S, E, Proj>;

    template <typename Rng, typename E, typename Proj>
    concept findable_last_range =
        input_range<Rng> and
        findable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms {
    struct find_last_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
            requires concepts::findable_last_iters_optimized<I, S, E, Proj>
        constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            auto result = last;
            for (; last != first; --last) {
                if (std::invoke(std::forward<Proj>(proj), *std::prev(last)) == elem) {
                    return std::prev(last);
                }
            }
            return result;
        }

        template <typename I, typename S, typename E, typename Proj = meta::identity>
            requires concepts::findable_last_iters_unoptimized<I, S, E, Proj>
        constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            auto result = last;
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Proj>(proj), *first) == elem) {
                    result = first;
                }
            }
            return result;
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
            requires concepts::findable_last_range<Rng, E, Proj>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    EXPORT_GENEX_STRUCT(find_last);
}
