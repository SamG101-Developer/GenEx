#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I>
    concept prevable_iters =
        std::bidirectional_iterator<I> and
        requires(I it, std::ptrdiff_t n) { std::prev(std::move(it), n); };
}


namespace genex::iterators {
    struct prev_fn {
        template <typename I> requires concepts::prevable_iters<I>
        auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::prev(std::move(it), n);
        }

        template <typename I> requires (concepts::prevable_iters<I> and std::random_access_iterator<I>)
        auto operator()(I it, const std::ptrdiff_t n, const I begin_it) const noexcept -> auto {
            auto res = std::prev(std::move(it), n);
            if (res < begin_it) { return begin_it; }
            return res;
        }
    };

    EXPORT_GENEX_STRUCT(prev);
}
