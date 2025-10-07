#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I, typename S = I>
    concept nextable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;
}


namespace genex::iterators {
    struct next_fn {
        template <typename I>
        requires concepts::nextable_iters<I> and (not std::random_access_iterator<I>)
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            for (std::ptrdiff_t i = 0; i < n; ++i, ++it);
            return it;
        }

        template <typename I>
        requires concepts::nextable_iters<I> and std::random_access_iterator<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return it + n;
        }

        template <typename I, typename S>
        requires concepts::nextable_iters<I, S> and (not std::random_access_iterator<I>)
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n, const S end_it) const noexcept -> auto {
            for (std::ptrdiff_t i = 0; i < n and it != end_it; ++i, ++it);
            return it;
        }

        template <typename I, typename S>
        requires concepts::nextable_iters<I, S> and std::random_access_iterator<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n, const S end_it) const noexcept -> auto {
            auto remaining = end_it - it;
            return it + (n <= remaining ? n : remaining);
        }
    };

    GENEX_EXPORT_STRUCT(next);
}
