#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/distance.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept can_position_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_position_last_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_position_last_iters_optimized =
        std::bidirectional_iterator<I> and
        can_position_last_iters<I, S, Pred, Proj>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_position_range =
        input_range<Rng> and
        can_position_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_position_last_range =
        forward_range<Rng> and
        can_position_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(position) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_position_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const ssize_t def = -1z, const ssize_t drop = 0z) const -> auto {
            for (auto pos = drop; first != last; ++first, ++pos) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    return pos;
                }
            }
            return def;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_position_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const ssize_t def = -1z, const ssize_t drop = 0z) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def, drop);
        }
    };

    DEFINE_ALGORITHM(position_last) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_position_last_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const ssize_t def = -1) const -> auto {
            auto pos = def;
            for (auto i = 0z; first != last; ++first, ++i) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    pos = i;
                }
            }
            return pos;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_position_last_iters_optimized<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const ssize_t def = -1) const -> auto {
            auto pos = static_cast<ssize_t>(iterators::distance(first, last));
            while (first != last) {
                --last;
                --pos;
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *last))) {
                    return pos;
                }
            }
            return def;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_position_last_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const ssize_t def = -1) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
        }
    };

    EXPORT_GENEX_ALGORITHM(position);
    EXPORT_GENEX_ALGORITHM(position_last);
}
