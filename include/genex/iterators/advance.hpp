#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I, typename S = I>
    concept advancable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        requires(I &it, iter_difference_t<I> n) { std::advance(it, n); };
}


namespace genex::iterators {
    struct advance_fn {
        template <typename I, typename S>
        requires concepts::advancable_iters<I, S> and std::random_access_iterator<I>
        GENEX_INLINE constexpr auto operator()(I &it, const iter_difference_t<I> n, S st) const -> I {
            it += std::min(n, st - it);
            return it;
        }

        template <typename I, typename S>
        requires concepts::advancable_iters<I, S> and (!std::random_access_iterator<I>)
        GENEX_INLINE constexpr auto operator()(I &it, const iter_difference_t<I> n, S st) const -> I {
            for (iter_difference_t<I> i = 0; i < n and it != st; ++i, ++it);
            return it;
        }
    };

    inline constexpr advance_fn advance{};
}
