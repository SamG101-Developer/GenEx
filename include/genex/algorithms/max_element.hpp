#pragma once
#include <genex/concepts.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept maxable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_strict_weak_order<Comp, std::projected<I, Proj>>;

    template <typename Rng, typename Comp, typename Proj>
    concept maxable_range =
        input_range<Rng> and
        maxable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Comp, typename Proj>
    requires concepts::maxable_iters<I, S, Comp, Proj>
    GENEX_INLINE constexpr auto do_max_element(I first, S last, Comp &&comp, Proj &&proj) -> iter_reference_t<I> {
        if (first == last) { return *first; }
        for (auto next = first; ++next != last;) {
            if (meta::invoke(comp, meta::invoke(proj, *next), meta::invoke(proj, *first))) { first = next; }
        }
        return *first;
    }
}


namespace genex {
    struct max_element_fn {
        template <typename I, typename S, typename Comp = operations::gt, typename Proj = meta::identity>
        requires algorithms::detail::concepts::maxable_iters<I, S, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> iter_reference_t<I> {
            return algorithms::detail::impl::do_max_element(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Comp = operations::gt, typename Proj = meta::identity>
        requires algorithms::detail::concepts::maxable_range<Rng, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> range_reference_t<Rng> {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_max_element(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    inline constexpr max_element_fn max_element{};
}
