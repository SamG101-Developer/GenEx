#pragma once
#include <iterator>
#include <genex/categories.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/iterators/advance.hpp>
#include <genex/macros.hpp>

using namespace genex::concepts;


namespace genex::iterators {
    template <typename I>
    concept has_std_next = requires(I &&it, std::ptrdiff_t n) { std::next(std::forward<I>(it), n); };

    template <typename I>
    concept has_next = has_std_next<I> || has_operator_plusplus<I>;

    template <typename I>
    concept has_std_prev = requires(I &&it, std::ptrdiff_t n) { std::prev(std::forward<I>(it), n); };

    template <typename I>
    concept has_prev = has_std_prev<I> || has_operator_minusminus<I>;

    DEFINE_ITERATOR(next) {
        template <typename I>
            requires (iterator<std::remove_cvref_t<I>> and categories::input_iterator<I> and has_std_next<I>)
        constexpr auto operator()(I &&it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::next(std::forward<I>(it), n);
        }

        template <iterator I>
            requires (iterator<std::remove_cvref_t<I>> and categories::input_iterator<I> and not has_std_next<I> and has_operator_plusplus<I>)
        constexpr auto operator()(I &&it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            auto copy_it = std::forward<I>(it);
            for (std::ptrdiff_t i = 0; i < n; ++i) { ++copy_it; }
            return copy_it;
        }
    };

    DEFINE_ITERATOR(prev) {
        template <typename I>
            requires (iterator<std::remove_cvref_t<I>> and categories::bidirectional_iterator<I> and has_std_prev<I>)
        constexpr auto operator()(I &&it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            return std::prev(std::forward<I>(it), n);
        }

        template <typename I>
            requires (iterator<std::remove_cvref_t<I>> and categories::bidirectional_iterator<I> and not has_std_prev<I> and has_operator_minusminus<I>)
        constexpr auto operator()(I &&it, const std::ptrdiff_t n = 1) const noexcept -> auto {
            CONSTRAIN_ITER_TAG(I, bidirectional_iterator);
            auto copy_it = std::forward<I>(it);
            for (std::ptrdiff_t i = 0; i < n; ++i) { --copy_it; }
            return copy_it;
        }
    };

    EXPORT_GENEX_ITERATOR(next);
    EXPORT_GENEX_ITERATOR(prev);
}
