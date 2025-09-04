#pragma once
#include <genex/algorithms/find.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept containable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_comparable<I, const std::remove_cvref_t<E>*, operations::eq, Proj, meta::identity>;

    template <typename Rng, typename E, typename Proj>
    concept containable_range =
        input_range<Rng> and
        containable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms {
    struct contains_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
            requires concepts::containable_iters<I, S, E, Proj>
        constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            auto it = algorithms::find(
                std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
            return it != last;
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
            requires concepts::containable_range<Rng, E, Proj>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(contains);
}
