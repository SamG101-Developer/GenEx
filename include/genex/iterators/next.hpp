#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I, typename S = I>
    concept nextable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        requires(I it, std::ptrdiff_t n) { std::next(std::move(it), n); };
}


namespace genex::iterators {
    struct next_fn {
        template <typename I>
        requires concepts::nextable_iters<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::next(std::move(it), n);
        }

        template <typename I, typename S>
        requires concepts::nextable_iters<I, S> and (not std::random_access_iterator<I>)
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n, const S end_it) const noexcept -> auto {
            return std::next(std::move(it), n);
        }

        template <typename I, typename S>
        requires concepts::nextable_iters<I, S> and std::random_access_iterator<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n, const S end_it) const noexcept -> auto {
            auto res = std::next(std::move(it), n);
            if (res > end_it) { return end_it; }
            return res;
        }
    };

    GENEX_EXPORT_STRUCT(next);
}
