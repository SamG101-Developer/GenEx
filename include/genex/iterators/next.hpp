#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/macros.hpp>


namespace genex::iterators {
    template <typename I>
    concept can_next_with_std =
        input_iterator<std::remove_cvref_t<I>> and
        requires(I it, std::ptrdiff_t n) { std::next(std::forward<I>(it), n); };

    template <typename I>
    concept can_prev_with_std =
        bidirectional_iterator<std::remove_cvref_t<I>> and
        requires(I it, std::ptrdiff_t n) { std::prev(std::forward<I>(it), n); };

    DEFINE_ITERATOR(next) {
        template <typename I> requires can_next_with_std<I>
        constexpr auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::next(std::move(it), n);
        }
    };

    DEFINE_ITERATOR(prev) {
        template <typename I> requires can_prev_with_std<I>
        constexpr auto operator()(I it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::prev(std::move(it), n);
        }
    };

    EXPORT_GENEX_ITERATOR(next);
    EXPORT_GENEX_ITERATOR(prev);
}
