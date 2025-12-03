module;
#include <genex/macros.hpp>

export module genex.algorithms.find_if_not;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import std;


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_if_not_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename E, typename Proj>
    concept findable_if_not_range =
        input_range<Rng> and
        findable_if_not_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::findable_if_not_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto do_find_if_not(I first, S last, Pred &&pred, Proj &&proj) -> I {
        for (; first != last; ++first) {
            if (not meta::invoke(pred, meta::invoke(proj, *first))) { break; }
        }
        return first;
    }
}


namespace genex {
    struct find_if_not_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires algorithms::detail::concepts::findable_if_not_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> I {
            return algorithms::detail::impl::do_find_if_not(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires algorithms::detail::concepts::findable_if_not_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> iterator_t<Rng> {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_find_if_not(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    export inline constexpr find_if_not_fn find_if_not{};
}
