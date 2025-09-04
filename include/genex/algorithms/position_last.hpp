#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Pred, typename Proj, typename Int>
    concept positionable_last_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        integer_like<Int>;

    template <typename I, typename S, typename Pred, typename Proj, typename Int>
    concept positionable_last_iters_optimized =
        std::bidirectional_iterator<I> and
        positionable_last_iters<I, S, Pred, Proj, Int>;

    template <typename I, typename S, typename Pred, typename Proj, typename Int>
    concept positionable_last_iters_unoptimized =
        not positionable_last_iters_optimized<I, S, Pred, Proj, Int> and
        positionable_last_iters<I, S, Pred, Proj, Int>;

    template <typename Rng, typename Pred, typename Proj, typename Int>
    concept positionable_last_range =
        forward_range<Rng> and
        positionable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj, Int>;
}


namespace genex::algorithms {
    struct position_last_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity, typename Int>
            requires concepts::positionable_last_iters<I, S, Pred, Proj, Int>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const Int def = -1z) const -> auto {
            auto pos = def;
            for (Int i = 0; first != last; ++first, ++i) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    pos = i;
                }
            }
            return pos;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity, typename Int>
            requires concepts::positionable_last_iters_optimized<I, S, Pred, Proj, Int>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const Int def = -1z) const -> auto {
            auto pos = static_cast<Int>(iterators::distance(first, last));
            while (first != last) {
                --last;
                --pos;
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *last))) {
                    return pos;
                }
            }
            return def;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity, typename Int = ssize_t>
            requires concepts::positionable_last_range<Rng, Pred, Proj, Int>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const Int def = -1z) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
        }
    };

    GENEX_EXPORT_STRUCT(position_last);
}
