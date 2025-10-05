#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/advance.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept can_slice_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::movable<I>;

    template <typename Rng, typename Int>
    concept can_slice_range =
        input_range<Rng> and
        can_slice_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
    requires concepts::can_slice_iters<I, S, Int>
    struct slice_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(slice_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(slice_iterator);

        Int step;

        GENEX_INLINE constexpr explicit slice_iterator(I it, S st, Int step) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)), step(std::max(1, step)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> slice_iterator& {
            iterators::advance(it, step, st);
            return *this;
        }
    };

    template <typename S, typename Int>
    struct slice_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(slice_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(slice_iterator, slice_sentinel, Int);
    };

    template <typename V, typename Int>
    requires concepts::can_slice_range<V, Int>
    struct slice_view {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(slice_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(slice_iterator, slice_sentinel, Int);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(step);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        Int start_idx;
        Int end_idx;
        Int step;

        GENEX_INLINE constexpr explicit slice_view(V rng, Int start_idx, Int end_idx, Int step) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<Int>) :
            base_rng(std::move(rng)), start_idx(std::max(0, start_idx)), end_idx(std::max(0, end_idx)), step(std::max(1, step)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng)) and noexcept(operations::size(base_rng))) {
            return iterators::next(iterators::begin(base_rng), start_idx, iterators::end(base_rng));
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::begin(base_rng)) and noexcept(iterators::end(base_rng))) {
            const auto dist = operations::size(base_rng);
            const auto cut = dist > end_idx ? dist - end_idx : 0;
            return iterators::next(iterators::begin(base_rng), start_idx + cut + 1, iterators::end(base_rng));
        }
    };
}


namespace genex::views {
    struct slice_fn {
        template <typename Rng, typename Int>
        requires detail::concepts::can_slice_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int start_idx, Int end_idx, Int step = static_cast<Int>(1)) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::slice_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), std::move(start_idx), std::move(end_idx), std::move(step)};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::can_slice_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int end_idx) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::slice_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), static_cast<Int>(0), std::move(end_idx), static_cast<Int>(1)};
        }

        template <typename Int>
        requires (not range<Int>)
        GENEX_INLINE constexpr auto operator()(Int start_idx, Int end_idx, Int step = static_cast<Int>(1)) const -> auto {
            return std::bind_back(
                slice_fn{}, std::move(start_idx), std::move(end_idx), std::move(step));
        }

        template <typename Int>
        requires (not range<Int>)
        GENEX_INLINE constexpr auto operator()(Int end_idx) const noexcept -> auto {
            return std::bind_back(
                slice_fn{}, static_cast<Int>(0), std::move(end_idx), static_cast<Int>(1));
        }
    };

    GENEX_EXPORT_STRUCT(slice);
}
