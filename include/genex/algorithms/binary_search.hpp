#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename Comp, typename Proj>
    concept binary_searchable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_strict_weak_order<Comp, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename Rng, typename E, typename Comp, typename Proj>
    concept binary_searchable_range =
        forward_range<Rng> and
        binary_searchable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Comp, Proj>;
}


namespace genex::algorithms {
    struct binary_search_fn {
        template <typename I, typename S, typename E, typename Comp = operations::lt, typename Proj = meta::identity>
            requires concepts::binary_searchable_iters<I, S, E, Comp, Proj>
        constexpr auto operator()(I first, S last, E &&elem, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            first = std::lower_bound(std::forward<I>(first), std::forward<S>(last), std::forward<E>(elem));
            return first != last && !std::invoke(
                std::forward<Comp>(comp), std::invoke(std::forward<Proj>(proj), *first), std::forward<E>(elem));
        }

        template <typename Rng, typename E, typename Comp = operations::lt, typename Proj = meta::identity>
            requires concepts::binary_searchable_range<Rng, E, Comp, Proj>
        constexpr auto operator()(Rng &&rng, E &&elem, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<E>(elem), std::forward<Comp>(comp),
                std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(binary_search);
}
