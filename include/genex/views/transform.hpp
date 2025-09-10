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


namespace genex::views::detail {
    template <typename I, typename S, typename F, typename Proj>
        requires concepts::transformable_iters<I, S, F, Proj>
    GENEX_NO_ASAN
    auto do_transform(I first, S last, F &&f, Proj &&proj) -> generator<std::invoke_result_t<F, std::invoke_result_t<Proj, iter_value_t<I>>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(f), std::invoke(std::forward<Proj>(proj), *first));
        }
    }
}


namespace genex::views {
    struct transform_fn {
        template <typename I, typename S, typename F, typename Proj = meta::identity>
            requires concepts::transformable_iters<I, S, F, Proj>
        constexpr auto operator()(I first, S last, F &&f, Proj &&proj = {}) const -> auto {
            return detail::do_transform(
                std::move(first), std::move(last), std::forward<F>(f), std::forward<Proj>(proj));
        }

        template <typename Rng, typename F, typename Proj> requires concepts::transformable_range<Rng, F, Proj>
        constexpr auto operator()(Rng &&rng, F &&f, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<F>(f), std::forward<Proj>(proj));
        }

        template <typename F, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<F>>)
        constexpr auto operator()(F &&f, Proj &&proj = {}) const -> auto {
            return std::bind_back(
                transform_fn{}, std::forward<F>(f), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(transform);
}
