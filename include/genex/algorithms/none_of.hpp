#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms {
    template <typename I, typename S, typename Pred, typename Proj>
    concept can_none_of_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<Proj, iter_value_t<I>> and
        std::predicate<Pred, std::invoke_result_t<Proj, iter_value_t<I>>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_none_of_range =
        input_range<Rng> and
        std::invocable<Proj, range_value_t<Rng>> and
        std::predicate<Pred, std::invoke_result_t<Proj, range_value_t<Rng>>>;

    DEFINE_ALGORITHM(none_of) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires can_none_of_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                    return false;
                }
            }
            return true;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires can_none_of_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            for (auto &&x : rng) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) {
                    return false;
                }
            }
            return true;
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires can_none_of_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> bool {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_ALGORITHM(none_of);
}
