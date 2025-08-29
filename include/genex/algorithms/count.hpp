#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept can_count_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_unary_invocable<Proj, I> and
        std::indirectly_comparable<I, const std::remove_cvref_t<E>*, operations::eq, Proj, meta::identity>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_count_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename E, typename Proj>
    concept can_count_range =
        input_range<Rng> and
        can_count_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_count_if_range =
        input_range<Rng> and
        can_count_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(count) {
        template <typename I, typename S, typename E, typename Proj = meta::identity> requires concepts::can_count_iters<I, S, E, Proj>
        auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> size_t {
            auto count = static_cast<std::size_t>(0);
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Proj>(proj), *first) == elem) { ++count; }
            }
            return count;
        }

        template <typename Rng, typename E, std::invocable<E> Proj = meta::identity> requires concepts::can_count_range<Rng, E, Proj>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            auto count = static_cast<std::size_t>(0);
            for (auto &&x : rng) {
                if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == elem) { ++count; }
            }
            return count;
        }
    };

    DEFINE_ALGORITHM(count_if) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_count_if_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> size_t {
            auto count = static_cast<std::size_t>(0);
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { ++count; }
            }
            return count;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_count_if_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto count = static_cast<std::size_t>(0);
            for (auto &&x : rng) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { ++count; }
            }
            return count;
        }
    };

    EXPORT_GENEX_ALGORITHM(count);
    EXPORT_GENEX_ALGORITHM(count_if);
}
