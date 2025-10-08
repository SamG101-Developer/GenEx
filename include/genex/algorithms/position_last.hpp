#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj, typename Int>
    concept positionable_last_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        integer_like<Int>;

    template <typename Rng, typename Pred, typename Proj, typename Int>
    concept positionable_last_range =
        forward_range<Rng> and
        positionable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj, Int>;
}


namespace genex::algorithms {
    struct position_last_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity, typename Int>
        requires detail::concepts::positionable_last_iters<I, S, Pred, Proj, Int> and std::bidirectional_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const Int def = -1z) const -> Int {
            auto pos = static_cast<Int>(iterators::distance(first, last));
            while (first != last) {
                --last;
                --pos;
                if (std::invoke(pred, std::invoke(proj, *last))) { return pos; }
            }
            return def;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity, typename Int>
        requires detail::concepts::positionable_last_iters<I, S, Pred, Proj, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const Int def = -1z) const -> Int {
            auto pos = def;
            for (Int i = 0; first != last; ++first, ++i) {
                if (std::invoke(pred, std::invoke(proj, *first))) { pos = i; }
            }
            return pos;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity, typename Int = ssize_t>
        requires detail::concepts::positionable_last_range<Rng, Pred, Proj, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const Int def = -1z) const -> Int {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
        }
    };

    inline constexpr position_last_fn position_last{};
}
