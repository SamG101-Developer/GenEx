#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept takeable_while_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept takeable_while_range =
        input_range<Rng> and
        takeable_while_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::takeable_while_iters<I, S, Pred, Proj>
    auto do_take_while(I first, S last, Pred pred, Proj proj = {}) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            if (not meta::invoke(pred, meta::invoke(proj, *first))) {
                break;
            }
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct take_while_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::takeable_while_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const {
            return detail::coros::do_take_while(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::takeable_while_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_take_while(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Pred, typename Proj = meta::identity>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
            return meta::bind_back(take_while_fn{}, std::move(pred), std::move(proj));
        }
    };

    inline constexpr take_while_fn take_while{};
}
