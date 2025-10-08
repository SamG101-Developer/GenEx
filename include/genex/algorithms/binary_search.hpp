#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
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
        requires detail::concepts::binary_searchable_iters<I, S, E, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E elem, Comp comp = {}, Proj proj = {}) const -> bool {
            first = std::lower_bound(std::move(first), std::move(last), elem);
            return first != last and not std::invoke(comp, std::invoke(proj, *first), std::move(elem));
        }

        template <typename Rng, typename E, typename Comp = operations::lt, typename Proj = meta::identity>
        requires detail::concepts::binary_searchable_range<Rng, E, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem, Comp comp = {}, Proj proj = {}) const -> bool {
            auto [first, last] = iterators::iter_pair(rng);
            first = std::lower_bound(std::move(first), std::move(last), elem);
            return first != last and not std::invoke(comp, std::invoke(proj, *first), std::move(elem));
        }
    };

    inline constexpr binary_search_fn binary_search{};
}
