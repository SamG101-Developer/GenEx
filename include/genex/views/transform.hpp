#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename F, typename Proj = meta::identity>
    concept transformable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_unary_invocable<Proj, I> and
        std::indirectly_unary_invocable<F, std::projected<I, Proj>>;

    template <typename Rng, typename F, typename Proj = meta::identity>
    concept transformable_range =
        input_range<Rng> and
        transformable_iters<iterator_t<Rng>, sentinel_t<Rng>, F, Proj>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    auto do_transform(I first, S last, F f, Proj proj) -> generator<std::invoke_result_t<F, std::invoke_result_t<Proj, iter_value_t<I>>>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield std::invoke(f, std::invoke(proj, *first));
        }
    }
}


namespace genex::views {
    struct transform_fn {
        template <typename I, typename S, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_iters<I, S, F, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, F f, Proj proj = {}) const {
            return detail::coros::do_transform(std::move(first), std::move(last), std::move(f), std::move(proj));
        }

        template <typename Rng, typename F, typename Proj>
        requires detail::concepts::transformable_range<Rng, F, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_transform(std::move(first), std::move(last), std::move(f), std::move(proj));
        }

        template <typename F, typename Proj = meta::identity>
        requires (not range<F>)
        GENEX_INLINE constexpr auto operator()(F f, Proj proj = {}) const {
            return std::bind_back(transform_fn{}, std::move(f), std::move(proj));
        }
    };

    inline constexpr transform_fn transform{};
}
