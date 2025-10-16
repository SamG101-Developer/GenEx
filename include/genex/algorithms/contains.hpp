#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/algorithms/find.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept containable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        algorithms::detail::concepts::findable_iters<I, S, E, Proj> and
        std::indirectly_comparable<I, const std::remove_cvref_t<E>*, operations::eq, Proj, meta::identity>;

    template <typename Rng, typename E, typename Proj>
    concept containable_range =
        input_range<Rng> and
        containable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename E, typename Proj>
    requires concepts::containable_iters<I, S, E, Proj>
    GENEX_INLINE constexpr auto do_contains(I first, S last, E&& elem, Proj &&proj) -> bool {
        auto it = algorithms::find(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        return it != last;
    }
}


namespace genex::algorithms {
    struct contains_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires detail::concepts::containable_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E&& elem, Proj &&proj = {}) const -> bool {
            return detail::impl::do_contains(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires detail::concepts::containable_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E&& elem, Proj &&proj = {}) const -> bool {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_contains(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    inline constexpr contains_fn contains{};
}
