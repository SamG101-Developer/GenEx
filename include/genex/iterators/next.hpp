#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I>
    concept nextable_iters =
        std::input_iterator<I> and
        requires(I it, std::ptrdiff_t n) { std::next(std::move(it), n); };
}


namespace genex::iterators {
    struct next_fn {
        template <typename I> requires concepts::nextable_iters<I>
        auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::next(std::move(it), n);
        }

        template <typename I> requires (concepts::nextable_iters<I> and std::random_access_iterator<I>)
        auto operator()(I it, const std::ptrdiff_t n, const I end_it) const noexcept -> auto {
            auto res = std::next(std::move(it), n);
            if (res > end_it) { return end_it; }
            return res;
        }
    };

    GENEX_EXPORT_STRUCT(next);
}
