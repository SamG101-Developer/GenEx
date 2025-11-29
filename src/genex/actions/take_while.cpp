module;
#include <genex/macros.hpp>

export module genex.actions.take_while;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.actions.erase;
import genex.algorithms.find_if_not;
import genex.iterators.access;
import std;


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Pred>
    concept takeable_while_range =
        forward_range<Rng> and
        std::indirect_unary_predicate<Pred, iterator_t<Rng>>;
}


namespace genex::actions {
    struct take_while_fn {
        template <typename Rng, typename Pred>
        requires detail::concepts::takeable_while_range<Rng, Pred>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred) const -> decltype(auto) {
            actions::erase(rng, iterators::begin(rng), genex::find_if_not(iterators::begin(rng), iterators::end(rng), pred));
            return std::forward<Rng>(rng);
        }

        template <typename Pred>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred) const {
            return meta::bind_back(take_while_fn{}, std::forward<Pred>(pred));
        }
    };

    export inline constexpr take_while_fn take_while{};
}
