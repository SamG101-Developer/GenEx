#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/prev.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_not_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::convertible_to<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename E, typename Proj>
    concept findable_last_if_not_range =
        input_range<Rng> and
        findable_last_if_not_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms {
    struct find_last_if_not_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::findable_last_if_not_iters<I, S, Pred, Proj> and std::bidirectional_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const -> I {
            auto true_last = last;
            for (; last != first; --last) {
                if (not std::invoke(pred, std::invoke(proj, *iterators::prev(last)))) { return iterators::prev(last); }
            }
            return true_last;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::findable_last_if_not_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const -> I {
            auto found_last = last;
            for (; first != last; ++first) {
                if (not std::invoke(pred, std::invoke(proj, *first))) { found_last = first; }
            }
            return found_last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::findable_last_if_not_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const -> iterator_t<Rng> {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }
    };

    inline constexpr find_last_if_not_fn find_last_if_not{};
}
