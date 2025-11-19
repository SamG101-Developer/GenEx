#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/prev.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept findable_last_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::convertible_to<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename E, typename Proj>
    concept findable_last_if_range =
        input_range<Rng> and
        findable_last_if_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::findable_last_if_iters<I, S, Pred, Proj> and std::bidirectional_iterator<I>
    GENEX_INLINE constexpr auto do_find_last_if(I first, S last, Pred &&pred, Proj &&proj) -> I {
        auto result = last;
        for (; last != first; --last) {
            if (meta::invoke(pred, meta::invoke(proj, *iterators::prev(last)))) { return iterators::prev(last); }
        }
        return result;
    }

    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::findable_last_if_iters<I, S, Pred, Proj>
    GENEX_INLINE constexpr auto do_find_last_if(I first, S last, Pred &&pred, Proj &&proj) -> I {
        auto result = last;
        for (; first != last; ++first) {
            if (meta::invoke(pred, meta::invoke(proj, *first))) { result = first; }
        }
        return result;
    }
}


namespace genex {
    struct find_last_if_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires algorithms::detail::concepts::findable_last_if_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> I {
            return algorithms::detail::impl::do_find_last_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires algorithms::detail::concepts::findable_last_if_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> iterator_t<Rng> {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_find_last_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    inline constexpr find_last_if_fn find_last_if{};
}
