#pragma once
#include <genex/concepts.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept sortabled_iters =
        std::random_access_iterator<I> and
        std::sentinel_for<S, I> and
        std::sortable<I, Comp, Proj>;

    template <typename Rng, typename Comp, typename Proj>
    concept sortabled_range =
        std::ranges::random_access_range<Rng> and
        sortabled_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Comp, typename Proj>
    requires concepts::sortabled_iters<I, S, Comp, Proj>
    GENEX_INLINE constexpr auto do_sorted(I first, S last, Comp &&comp, Proj &&proj) -> std::vector<iter_value_t<I>> {
        auto vec = std::vector<iter_value_t<I>>(std::make_move_iterator(first), std::make_move_iterator(last));
        std::sort(vec.begin(), vec.end(), [comp, proj]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
            return meta::invoke(comp, meta::invoke(proj, std::forward<Lhs>(lhs)), meta::invoke(proj, std::forward<Rhs>(rhs)));
        });
        return vec;
    }
}


namespace genex {
    struct sorted_fn {
        template <typename I, typename S, typename Comp = operations::lt, typename Proj = meta::identity>
        requires algorithms::detail::concepts::sortabled_iters<I, S, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> std::vector<iter_value_t<I>> {
            return algorithms::detail::impl::do_sorted(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
        requires algorithms::detail::concepts::sortabled_range<Rng, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> std::vector<range_value_t<Rng>> {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_sorted(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    inline constexpr sorted_fn sorted{};
}
