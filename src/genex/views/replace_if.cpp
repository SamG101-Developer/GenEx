module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.replace_if;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
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


namespace genex::views::detail::impl {
    template <typename I, typename S, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    auto do_replace_if(I first, S last, Pred pred, New new_val, Proj proj) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            if (meta::invoke(pred, meta::invoke(proj, *first))) { co_yield new_val; }
            else { co_yield static_cast<iter_value_t<I>>(*first); }
        }
    }
}


namespace genex::views {
    struct replace_if_fn {
        template <typename I, typename S, typename Pred, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_if_iters<I, S, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, New new_val, Proj proj = {}) const {
            return detail::impl::do_replace_if(std::move(first), std::move(last), std::move(pred), std::move(new_val), std::move(proj));
        }

        template <typename Rng, typename Pred, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, New new_val, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_replace_if(std::move(first), std::move(last), std::move(pred), std::move(new_val), std::move(proj));
        }

        template <typename Pred, typename New, typename Proj = meta::identity>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred, New new_val, Proj proj = {}) const {
            return meta::bind_back(replace_if_fn{}, std::move(pred), std::move(new_val), std::move(proj));
        }
    };

    export inline constexpr replace_if_fn replace_if{};
}
