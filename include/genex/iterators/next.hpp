#pragma once
#include <iterator>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I>
    concept can_next_iters =
        std::input_iterator<I> and
        std::movable<I> and
        requires(I it, std::ptrdiff_t n) { std::next(std::move(it), n); };

    template <typename I>
    concept can_prev_iters =
        std::bidirectional_iterator<I> and
        std::movable<I> and
        requires(I it, std::ptrdiff_t n) { std::prev(std::move(it), n); };
}


namespace genex::iterators {
    DEFINE_ITERATOR(next) {
        template <typename I> requires concepts::can_next_iters<I>
        constexpr auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::next(std::move(it), n);
        }
    };

    DEFINE_ITERATOR(prev) {
        template <typename I> requires concepts::can_prev_iters<I>
        constexpr auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::prev(std::move(it), n);
        }
    };

    EXPORT_GENEX_ITERATOR(next);
    EXPORT_GENEX_ITERATOR(prev);
}
