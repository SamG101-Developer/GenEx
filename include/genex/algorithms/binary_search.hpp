#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename Comp, typename Proj>
    concept can_binary_search_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_strict_weak_order<Comp, std::projected<I, Proj>, const E*>;

    template <typename Rng, typename E, typename Comp, typename Proj>
    concept can_binary_search_range =
        forward_range<Rng> and
        can_binary_search_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Comp, Proj>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(binary_search) {
        template <typename I, typename S, typename E, typename Comp = operations::lt, typename Proj = meta::identity> requires concepts::can_binary_search_iters<I, S, E, Comp, Proj>
        auto operator()(I first, S last, E &&elem, Comp &&comp = {}, Proj &&proj = {}) const -> bool {
            first = std::lower_bound(std::forward<I>(first), std::forward<S>(last), std::forward<E>(elem));
            return first != last && !std::invoke(std::forward<Comp>(comp), std::invoke(std::forward<Proj>(proj), *first), std::forward<E>(elem));
        }

        template <typename Rng, typename E, typename Comp = operations::lt, typename Proj = meta::identity> requires concepts::can_binary_search_range<Rng, E, Comp, Proj>
        auto operator()(Rng &&rng, E &&elem, Comp &&comp = {}, Proj &&proj = {}) const -> bool {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<E>(elem), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename E, typename Comp = operations::lt, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<E>>)
        auto operator()(E &&elem, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(elem, comp, proj)]<typename Rng> requires concepts::can_binary_search_range<Rng, E, Comp, Proj>
                (Rng &&rng) mutable -> bool {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem), std::forward<Comp>(comp), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_ALGORITHM(binary_search);
}
