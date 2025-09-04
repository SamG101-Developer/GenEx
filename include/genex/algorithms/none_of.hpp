#pragma once
#include <genex/algorithms/_concepts.hpp>
#include <genex/algorithms/find_if.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms {
    struct none_of_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
            requires concepts::quantifiable_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Check for an iterator that does satisfy the predicate; if one exists, an elements satisfies it.
            auto it = algorithms::find_if(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return it == last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
            requires concepts::quantifiable_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Convert the range into iterators and call the iterator overload.
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    EXPORT_GENEX_STRUCT(none_of);
}
