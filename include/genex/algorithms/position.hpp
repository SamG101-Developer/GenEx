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
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const std::make_signed_t<std::size_t> def = -1z) const -> auto {
            for (auto pos = 0z; first != last; ++first, ++pos) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    return pos;
                }
            }
            return def;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_position_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const std::make_signed_t<std::size_t> def = -1z) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        auto operator()(Pred &&pred, Proj &&proj = {}, const std::make_signed_t<std::size_t> def = -1z) const -> auto {
            return
                [FWD_CAPTURES(pred, proj, def)]<typename Rng> requires concepts::can_position_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
            };
        }
    };

    DEFINE_ALGORITHM(position_last) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_position_last_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const std::make_signed_t<std::size_t> def = -1) const -> auto {
            auto pos = def;
            for (auto i = 0z; first != last; ++first, ++i) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    pos = i;
                }
            }
            return pos;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_position_last_iters_optimized<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}, const std::make_signed_t<std::size_t> def = -1) const -> auto {
            auto pos = static_cast<std::make_signed_t<std::size_t>>(iterators::distance(first, last));
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
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, const std::make_signed_t<std::size_t> def = -1) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        auto operator()(Pred &&pred, Proj &&proj = {}, const std::make_signed_t<std::size_t> def = -1) const -> auto {
            return
                [FWD_CAPTURES(pred, proj, def)]<typename Rng> requires concepts::can_position_last_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
            };
        }
    };

    EXPORT_GENEX_ALGORITHM(position);
    EXPORT_GENEX_ALGORITHM(position_last);
}
