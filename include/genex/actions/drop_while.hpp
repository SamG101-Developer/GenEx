#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/algorithms/find_if_not.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename Pred>
    concept droppable_while_range =
        forward_range<Rng> and
        erasable_range<Rng&, iterator_t<Rng>, sentinel_t<Rng>> and
        std::indirect_unary_predicate<Pred, iterator_t<Rng>>;
}


namespace genex::actions {
    struct drop_while_fn {
        template <typename Rng, typename Pred>
            requires concepts::droppable_while_range<Rng, Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred) const -> decltype(auto) {
            actions::erase(
                rng, algorithms::find_if_not(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred)),
                iterators::end(rng));
            return std::forward<Rng>(rng);
        }

        template <typename Pred>
            requires (not range<Pred>)
        constexpr auto operator()(Pred &&pred) const -> auto {
            return std::bind_back(drop_while_fn{}, std::forward<Pred>(pred));
        }
    };

    GENEX_EXPORT_STRUCT(drop_while);
}
