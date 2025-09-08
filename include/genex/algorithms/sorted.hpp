#pragma once
#include <vector>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/meta.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept sortabled_iters =
        std::random_access_iterator<I> &&
        std::sentinel_for<S, I> &&
        std::sortable<I, Comp, Proj>;

    template <typename Rng, typename Comp, typename Proj>
    concept sortabled_range =
        std::ranges::random_access_range<Rng> &&
        sortabled_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::algorithms {
    struct sorted_fn {
        template <typename I, typename S, typename Comp = operations::lt, typename Proj = meta::identity>
            requires concepts::sortabled_iters<I, S, Comp, Proj>
        constexpr auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> decltype(auto) {
            // Create a new, sorted, range.
            auto vec = std::vector<iter_value_t<I>>(
                std::make_move_iterator(first),
                std::make_move_iterator(last));
            std::sort(vec.begin(), vec.end(), [comp, proj]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
                return std::invoke(std::move(comp), std::invoke(proj, std::forward<Lhs>(lhs)), std::invoke(proj, std::forward<Rhs>(rhs)));
            });
            return vec;
        }

        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
            requires concepts::sortabled_range<Rng, Comp, Proj>
        constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> decltype(auto) {
            // Get the iterators and call the iterator version.
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(sorted);
}
