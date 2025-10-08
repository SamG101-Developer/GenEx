#pragma once
#include <genex/concepts.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept minable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_strict_weak_order<Comp, std::projected<I, Proj>>;

    template <typename Rng, typename Comp, typename Proj>
    concept minable_range =
        input_range<Rng> and
        minable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::algorithms {
    struct min_element_fn {
        template <typename I, typename S, typename Comp = operations::lt, typename Proj = meta::identity>
        requires detail::concepts::minable_iters<I, S, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> iter_reference_t<I> {
            if (first == last) { return *first; }
            for (auto next = first; ++next != last;) {
                if (meta::invoke(comp, meta::invoke(proj, *next), meta::invoke(proj, *first))) { first = next; }
            }
            return *first;
        }

        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
        requires detail::concepts::minable_range<Rng, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> range_reference_t<Rng> {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    inline constexpr min_element_fn min_element{};
}
