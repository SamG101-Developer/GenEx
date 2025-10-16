#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/prev.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept findable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::convertible_to<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename Rng, typename E, typename Proj>
    concept findable_last_range =
        input_range<Rng> and
        findable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename E, typename Proj>
    requires concepts::findable_last_iters<I, S, E, Proj> and std::bidirectional_iterator<I>
    GENEX_INLINE constexpr auto do_find_last(I first, S last, E &&elem, Proj &&proj) -> I {
        auto result = last;
        for (; last != first; --last) {
            if (meta::invoke(proj, *iterators::prev(last)) == elem) { return iterators::prev(last); }
        }
        return result;
    }

    template <typename I, typename S, typename E, typename Proj>
    requires concepts::findable_last_iters<I, S, E, Proj>
    GENEX_INLINE constexpr auto do_find_last(I first, S last, E &&elem, Proj &&proj) -> I {
        auto result = last;
        for (; first != last; ++first) {
            if (meta::invoke(proj, *first) == elem) { result = first; }
        }
        return result;
    }
}


namespace genex::algorithms {
    struct find_last_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires detail::concepts::findable_last_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> I {
            return detail::impl::do_find_last(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires detail::concepts::findable_last_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> iterator_t<Rng> {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_find_last(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    inline constexpr find_last_fn find_last{};
}
