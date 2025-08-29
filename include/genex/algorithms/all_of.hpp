#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept can_all_of_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_all_of_range =
        input_range<Rng> and
        can_all_of_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(all_of) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_all_of_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> bool {
            for (; first != last; ++first) {
                if (not std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    return false;
                }
            }
            return true;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_all_of_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> bool {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    EXPORT_GENEX_ALGORITHM(all_of);
}
