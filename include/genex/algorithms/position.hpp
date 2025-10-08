#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>


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


namespace genex::algorithms {
    struct position_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity, typename Int>
        requires detail::concepts::positionable_iters<I, S, Pred, Proj, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const Int def = -1z, const std::size_t drop = 0z) const -> Int {
            for (Int pos = drop; first != last; ++first, ++pos) {
                if (std::invoke(pred, std::invoke(proj, *first))) { return pos; }
            }
            return def;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity, typename Int = ssize_t>
        requires detail::concepts::positionable_range<Rng, Pred, Proj, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const Int def = -1z, const ssize_t drop = 0z) const -> Int {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj), def, drop);
        }
    };


    inline constexpr position_fn position{};
}
