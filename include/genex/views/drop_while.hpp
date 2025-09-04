#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept droppable_while_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept droppable_while_range =
        input_range<Rng> and
        droppable_while_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Pred, typename Proj>
        requires concepts::droppable_while_iters<I, S, Pred, Proj>
    auto do_drop_while(I first, S last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        auto dropping = true;
        for (; first != last; ++first) {
            if (dropping && std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                continue;
            }
            dropping = false;
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct drop_while_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
            requires concepts::droppable_while_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            return detail::do_drop_while(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
            requires concepts::droppable_while_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity>
            requires (not range<Pred>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            return std::bind_back(drop_while_fn{}, std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    EXPORT_GENEX_STRUCT(drop_while);
}
