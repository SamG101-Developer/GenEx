module;
#include <genex/macros.hpp>

export module genex.algorithms.position;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj, typename Int>
    concept positionable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Pred, typename Proj, typename Int>
    concept positionable_range =
        input_range<Rng> and
        positionable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj, Int>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Pred, typename Proj, typename Int>
    requires concepts::positionable_iters<I, S, Pred, Proj, Int>
    GENEX_INLINE constexpr auto do_position(I first, S last, Pred &&pred, Proj &&proj, const Int def, const std::ptrdiff_t drop) -> Int {
        for (auto i = drop; first != last; ++first, ++i) {
            if (meta::invoke(pred, meta::invoke(proj, *first))) { return static_cast<Int>(i); }
        }
        return def;
    }
}


namespace genex {
    struct position_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity, typename Int>
        requires algorithms::detail::concepts::positionable_iters<I, S, Pred, Proj, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const Int def = -1z, const std::ptrdiff_t drop = 0z) const -> Int {
            return algorithms::detail::impl::do_position(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj), def, drop);
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity, typename Int = std::ptrdiff_t>
        requires algorithms::detail::concepts::positionable_range<Rng, Pred, Proj, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const Int def = -1z, const std::ptrdiff_t drop = 0z) const -> Int {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_position(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj), def, drop);
        }
    };


    export inline constexpr position_fn position{};
}
