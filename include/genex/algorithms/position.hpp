#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/distance.hpp>


namespace genex::algorithms {
    template <typename I, typename S, typename Pred, typename Proj>
    concept can_position_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_position_range =
        input_range<Rng> and
        std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_position_last_iters =
        forward_iterator<I> and
        sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_position_last_range =
        forward_range<Rng> and
        std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_position_last_iters_optimized =
        bidirectional_iterator<I> and
        can_position_last_iters<I, S, Pred, Proj>;

    DEFINE_ALGORITHM(position) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            for (auto pos = 0z; first != last; ++first, ++pos) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    return pos;
                }
            }
            return static_cast<std::make_signed_t<std::size_t>>(-1);
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires can_position_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    DEFINE_ALGORITHM(position_last) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires can_position_last_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto pos = -1z;
            for (auto i = 0z; first != last; ++first, ++i) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    pos = i;
                }
            }
            return pos;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires can_position_last_iters_optimized<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto pos = static_cast<std::make_signed_t<std::size_t>>(iterators::distance(first, last));
            for (auto it = last; it != first; --it, --pos) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *it))) {
                    return pos;
                }
            }
            return static_cast<std::make_signed_t<std::size_t>>(-1);
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires can_position_last_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires can_position_last_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_ALGORITHM(position);
    EXPORT_GENEX_ALGORITHM(position_last);
}
