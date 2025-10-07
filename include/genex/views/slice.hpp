#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/advance.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept sliceable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::movable<I>;

    template <typename Rng, typename Int>
    concept sliceable_range =
        input_range<Rng> and
        sliceable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
    requires concepts::sliceable_iters<I, S, Int>
    struct slice_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        Int step;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            slice_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            slice_iterator, it);

        GENEX_INLINE constexpr explicit slice_iterator(I it, S st, Int step) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Int>) :
            it(std::move(it)), st(std::move(st)), step(std::move(step)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(iterators::advance(it, step, st))) -> slice_iterator& {
            iterators::advance(it, step, st);
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> slice_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct slice_sentinel { };

    template <typename I, typename S, typename Int>
    requires concepts::sliceable_iters<I, S, Int>
    slice_iterator(I, S, Int) -> slice_iterator<I, S, Int>;

    template <typename I, typename S, typename Int>
    requires concepts::sliceable_iters<I, S, Int>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(slice_iterator, slice_sentinel, I, S, Int) {
        return it.it == it.st;
    }

    template <typename V, typename Int>
    requires concepts::sliceable_range<V, Int>
    struct slice_view {
        V base_rng;
        Int start_idx;
        Int end_idx;
        Int step;

        GENEX_INLINE constexpr explicit slice_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            slice_iterator, slice_sentinel, base_rng, step);

        GENEX_INLINE constexpr explicit slice_view(V rng, Int start_idx, Int end_idx, Int step) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, V, Int>) :
            base_rng(std::move(rng)), start_idx(std::move(start_idx)), end_idx(std::move(end_idx)),
            step(std::move(step)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng)) and noexcept(operations::size(base_rng))) {
            return iterators::next(iterators::begin(base_rng), start_idx, iterators::end(base_rng));
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::begin(base_rng)) and noexcept(iterators::end(base_rng))) {
            const auto dist = operations::size(base_rng);
            const auto cut = dist > end_idx ? dist - end_idx : 0;
            return iterators::next(iterators::begin(base_rng), start_idx + cut + 1, iterators::end(base_rng));
        }

        GENEX_INLINE constexpr auto size() const noexcept(
            noexcept(operations::size(base_rng))) {
            const auto total_size = operations::size(base_rng);
            if (end_idx <= start_idx or start_idx >= total_size) { return static_cast<range_size_t<V>>(0); }
            const auto actual_end = std::min(end_idx, static_cast<Int>(total_size));
            return static_cast<range_size_t<V>>((actual_end - start_idx + step - 1) / step);
        }
    };
}


namespace genex::views {
    struct slice_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::sliceable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I it, S st, Int start_idx, Int end_idx, Int step = static_cast<Int>(1)) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Int>) {
            using V = std::ranges::subrange<I, S>;
            return detail::slice_view<V, Int>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}, std::move(start_idx), std::move(end_idx), std::move(step)};
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::sliceable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I it, S st, Int end_idx) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Int>) {
            using V = std::ranges::subrange<I, S>;
            return detail::slice_view<V, Int>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}, static_cast<Int>(0), std::move(end_idx), static_cast<Int>(1)};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::sliceable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int start_idx, Int end_idx, Int step = static_cast<Int>(1)) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&> and
            meta::all_of_v<std::is_nothrow_move_constructible, Int>) {
            using V = std::views::all_t<Rng>;
            return detail::slice_view<V, Int>{
                std::forward<Rng>(rng), std::move(start_idx), std::move(end_idx), std::move(step)};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::sliceable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int end_idx) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&> and
            meta::all_of_v<std::is_nothrow_move_constructible, Int>) {
            using V = std::views::all_t<Rng>;
            return detail::slice_view<V, Int>{
                std::forward<Rng>(rng), static_cast<Int>(0), std::move(end_idx), static_cast<Int>(1)};
        }

        template <typename Int>
        requires (not range<Int>)
        GENEX_INLINE constexpr auto operator()(Int start_idx, Int end_idx, Int step = static_cast<Int>(1)) const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, slice_fn> and
            meta::all_of_v<std::is_nothrow_move_constructible, Int>) {
            return std::bind_back(
                slice_fn{}, std::move(start_idx), std::move(end_idx), std::move(step));
        }

        template <typename Int>
        requires (not range<Int>)
        GENEX_INLINE constexpr auto operator()(Int end_idx) const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, slice_fn> and
            meta::all_of_v<std::is_nothrow_move_constructible, Int>) {
            return std::bind_back(
                slice_fn{}, static_cast<Int>(0), std::move(end_idx), static_cast<Int>(1));
        }
    };

    inline constexpr slice_fn slice{};
}
