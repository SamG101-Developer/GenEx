#pragma once
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I, typename S = I>
    concept prevable_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        requires(I it, std::ptrdiff_t n) { std::prev(std::move(it), n); };
}


namespace genex::iterators {
    struct prev_fn {
        template <typename I>
        requires concepts::prevable_iters<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::prev(std::move(it), n);
        }

        template <typename I, typename S>
        requires concepts::prevable_iters<I, S> and (not std::random_access_iterator<I>)
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n, const S end_it) const noexcept -> auto {
            return std::prev(std::move(it), n, end_it);
        }

        template <typename I, typename S>
        requires concepts::prevable_iters<I> and std::random_access_iterator<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n, const S begin_it) const noexcept -> auto {
            auto res = std::prev(std::move(it), n);
            if (res < begin_it) { return begin_it; }
            return res;
        }
    };

    GENEX_EXPORT_STRUCT(prev);
}
