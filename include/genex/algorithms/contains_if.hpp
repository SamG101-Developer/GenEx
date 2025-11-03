#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/algorithms/find_if.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept containable_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        algorithms::detail::concepts::findable_if_iters<I, S, Pred, Proj> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept containable_if_range =
        input_range<Rng> and
        containable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::containable_if_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto do_contains(I first, S last, Pred &&pred, Proj &&proj) -> bool {
        auto it = algorithms::find_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        return it != last;
    }
}


namespace genex::algorithms {
    struct contains_if_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires detail::concepts::containable_if_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> bool {
            return detail::impl::do_contains(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires detail::concepts::containable_if_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> bool {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_contains(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    inline constexpr contains_if_fn contains_if{};
}
