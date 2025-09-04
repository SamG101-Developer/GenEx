#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Pred, typename New, typename Proj>
    concept replaceable_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::convertible_to<New, iter_value_t<I>>;

    template <typename Rng, typename Pred, typename New, typename Proj>
    concept replaceable_if_range =
        input_range<Rng> and
        replaceable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, New, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Pred, typename New, typename Proj>
        requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    auto do_replace_if(I first, S last, Pred &&pred, New &&new_val, Proj &&proj) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (std::invoke(pred, std::invoke(proj, *first))) { co_yield new_val; }
            else { co_yield static_cast<iter_value_t<I>>(*first); }
        }
    }
}


namespace genex::views {
    struct replace_if_fn {
        template <typename I, typename S, typename Pred, typename New, typename Proj = meta::identity>
            requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, New &&new_val, Proj &&proj = {}) const -> auto {
            return detail::do_replace_if(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<New>(new_val),
                std::forward<Proj>(proj));
        }

        template <typename Rng, typename Pred, typename New, typename Proj = meta::identity>
            requires concepts::replaceable_if_range<Rng, Pred, New, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, New &&new_val, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<New>(new_val),
                std::forward<Proj>(proj));
        }

        template <typename Pred, typename New, typename Proj = meta::identity>
            requires (not range<Pred>)
        constexpr auto operator()(Pred &&pred, New &&new_val, Proj &&proj = {}) const -> auto {
            return std::bind_back(
                replace_if_fn{}, std::forward<Pred>(pred), std::forward<New>(new_val), std::forward<Proj>(proj));
        }
    };

    EXPORT_GENEX_STRUCT(replace_if);
}
