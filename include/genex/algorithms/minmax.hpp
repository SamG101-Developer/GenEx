#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept can_minmax_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_strict_weak_order<Comp, std::projected<I, Proj>>;

    template <typename Rng, typename Comp, typename Proj>
    concept can_minmax_range =
        input_range<Rng> and
        can_minmax_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::algorithms::detail {
    template <typename I, typename S, typename F, typename Proj>
    auto do_cmp(I first, S last, F &&f, Proj &&proj) -> iter_value_t<I> {
        auto best_value = *first;
        auto best_proj_value = proj(best_value);

        for (++first; first != last; ++first) {
            auto current_value = *first;
            auto current_proj_value = proj(current_value);
            if (f(current_proj_value, best_proj_value)) {
                best_value = current_value;
                best_proj_value = current_proj_value;
            }
        }

        return best_value;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(min) {
        template <typename I, typename S, typename Comp = operations::lt, typename Proj = meta::identity> requires concepts::can_minmax_iters<I, S, Comp, Proj>
        auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return detail::do_cmp(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity> requires concepts::can_minmax_range<Rng, Comp, Proj>
        auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    DEFINE_ALGORITHM(max) {
        template <typename I, typename S, typename Comp = operations::gt, typename Proj = meta::identity> requires concepts::can_minmax_iters<I, S, Comp, Proj>
        auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return detail::do_cmp(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Comp = operations::gt, typename Proj = meta::identity> requires concepts::can_minmax_range<Rng, Comp, Proj>
        auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    EXPORT_GENEX_ALGORITHM(min);
    EXPORT_GENEX_ALGORITHM(max);
}
