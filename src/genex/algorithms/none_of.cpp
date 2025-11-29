module;
#include <genex/macros.hpp>

export module genex.algorithms.none_of;
import genex.meta;
import genex.algorithms.concepts;
import genex.algorithms.find_if;
import genex.iterators.iter_pair;
import std;


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::quantifiable_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto do_none_of(I first, S last, Pred &&pred, Proj &&proj) -> bool {
        auto it = genex::find_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        return it == last;
    }
}


namespace genex {
    struct none_of_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires algorithms::detail::concepts::quantifiable_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> bool {
            return algorithms::detail::impl::do_none_of(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires algorithms::detail::concepts::quantifiable_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> bool {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_none_of(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    export inline constexpr none_of_fn none_of{};
}
