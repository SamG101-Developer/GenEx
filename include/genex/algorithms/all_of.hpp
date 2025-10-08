#pragma once
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/algorithms/find_if_not.hpp>
#include <genex/algorithms/_concepts.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::algorithms {
    struct all_of_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::quantifiable_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> bool {
            auto it = algorithms::find_if_not(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return it == last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::quantifiable_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> bool {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    inline constexpr all_of_fn all_of{};
}
