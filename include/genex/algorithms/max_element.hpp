#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::concepts {
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


namespace genex::algorithms {
    struct max_element_fn {
        template <typename I, typename S, typename Comp = operations::gt, typename Proj = meta::identity>
            requires concepts::maxable_iters<I, S, Comp, Proj>
        constexpr auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            if (first == last) { return *first; }
            for (auto next = first; ++next != last;) {
                if (std::invoke(comp, std::invoke(proj, *next), std::invoke(proj, *first))) {
                    first = next;
                }
            }
            return *first;
        }

        template <typename Rng, typename Comp = operations::gt, typename Proj = meta::identity>
            requires concepts::maxable_range<Rng, Comp, Proj>
        constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(max_element);
}
