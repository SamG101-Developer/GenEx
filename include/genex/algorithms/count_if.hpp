#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept can_count_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_count_if_range =
        input_range<Rng> and
        can_count_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::can_count_if_iters<I, S, Pred, Proj>
    GENEX_INLINE auto do_count_if(I first, S last, Pred &&pred, Proj &&proj) -> std::size_t {
        auto count = 0uz;
        for (; first != last; ++first) {
            if (meta::invoke(pred, meta::invoke(proj, *first))) { ++count; }
        }
        return count;
    }
}


namespace genex::algorithms {
    struct count_if_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::can_count_if_iters<I, S, Pred, Proj>
        GENEX_INLINE auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> std::size_t {
            return detail::impl::do_count_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::can_count_if_range<Rng, Pred, Proj>
        GENEX_INLINE auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> std::size_t {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_count_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    inline constexpr count_if_fn count_if{};
}
