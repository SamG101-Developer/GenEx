module;
#include <genex/macros.hpp>

export module genex.actions.drop_while;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.actions.erase;
import genex.algorithms.find_if_not;
import genex.iterators.access;
import std;


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Pred>
    concept droppable_while_range =
        forward_range<Rng> and
        std::indirect_unary_predicate<Pred, iterator_t<Rng>>;
}


namespace genex::actions {
    struct drop_while_fn {
        template <typename Rng, typename Pred>
        requires detail::concepts::droppable_while_range<Rng, Pred>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred) const -> decltype(auto) {
            actions::erase(
                rng, genex::find_if_not(iterators::begin(rng), iterators::end(rng), std::move(pred)),
                iterators::end(rng));
            return std::forward<Rng>(rng);
        }

        template <typename Pred>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred) const {
            return meta::bind_back(drop_while_fn{}, std::move(pred));
        }
    };

    inline constexpr drop_while_fn drop_while{};
}
