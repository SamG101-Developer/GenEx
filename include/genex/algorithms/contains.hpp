#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept can_contain_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_comparable<I, const std::remove_cvref_t<E>*, operations::eq, Proj, meta::identity>;

    template <typename Rng, typename E, typename Proj>
    concept can_contain_range =
        input_range<Rng> and
        can_contain_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(contains) {
        template <typename I, typename S, typename E, typename Proj = meta::identity> requires concepts::can_contain_iters<I, S, E, Proj>
        auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> bool {
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Proj>(proj), *first) == std::forward<E>(elem)) { return true; }
            }
            return false;
        }

        template <typename Rng, typename E, typename Proj = meta::identity> requires concepts::can_contain_range<Rng, E, Proj>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> bool {
            for (auto &&x : rng) {
                if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == std::forward<E>(elem)) { return true; }
            }
            return false;
        }
    };

    EXPORT_GENEX_ALGORITHM(contains);
}
