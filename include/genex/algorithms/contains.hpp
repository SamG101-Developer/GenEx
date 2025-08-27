#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms {
    template <typename I, typename S, typename E, typename Proj>
    concept can_contain_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<Proj, iter_value_t<I>> and
        std::equality_comparable_with<std::invoke_result_t<Proj, iter_value_t<I>>, E>;

    template <typename Rng, typename E, typename Proj>
    concept can_contain_range =
        input_range<Rng> and
        std::invocable<Proj, range_value_t<Rng>> and
        std::equality_comparable_with<std::invoke_result_t<Proj, range_value_t<Rng>>, E>;

    DEFINE_ALGORITHM(contains) {
        template <typename I, typename S, typename E, typename Proj = meta::identity> requires can_contain_iters<I, S, E, Proj>
        constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> bool {
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Proj>(proj), *first) == std::forward<E>(elem)) { return true; }
            }
            return false;
        }

        template <typename Rng, typename E, typename Proj = meta::identity> requires can_contain_range<Rng, E, Proj>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> bool {
            for (auto &&x : rng) {
                if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == std::forward<E>(elem)) { return true; }
            }
            return false;
        }

        template <typename E, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<E>>)
        constexpr auto operator()(E &&elem, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(elem, proj)]<typename Rng> requires can_contain_range<Rng, E, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_ALGORITHM(contains);
}
