#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept can_sorted_iters =
        std::random_access_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::sortable<I, Comp, Proj>;

    template <typename Rng, typename Comp, typename Proj>
    concept can_sorted_range =
        random_access_range<Rng> and
        can_sorted_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(sorted) {
        template <typename I, typename S, typename Comp = operations::lt, typename Proj = meta::identity> requires concepts::can_sorted_iters<I, S, Comp, Proj>
        constexpr auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            auto copy = std::vector<iter_value_t<I>>();
            copy.reserve(iterators::distance(first, last));

            for (; first != last; ++first) {
                copy.push_back(*first);
            }

            std::sort(copy.begin(), copy.end(), [comp, proj]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
                return std::invoke(std::move(comp), std::invoke(proj, std::forward<Lhs>(lhs)), std::invoke(proj, std::forward<Rhs>(rhs)));
            });

            return std::move(copy);
        }

        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity> requires concepts::can_sorted_range<Rng, Comp, Proj>
        constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Comp = operations::lt, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Comp>>)
        constexpr auto operator()(Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(comp, proj)]<typename Rng> requires concepts::can_sorted_range<Rng, Comp, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Comp>(comp), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_ALGORITHM(sorted);
}
