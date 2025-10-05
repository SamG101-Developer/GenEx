#pragma once
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/next.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::movable<I>;

    template <typename Rng, typename Int>
    concept droppable_last_range =
        input_range<Rng> and
        droppable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
    requires concepts::droppable_last_iters<I, S, Int>
    struct drop_last_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(drop_last_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(drop_last_iterator);

        GENEX_INLINE constexpr explicit drop_last_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
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
            -> drop_last_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, typename Int>
    struct drop_last_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(drop_last_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(drop_last_iterator, drop_last_sentinel, Int);
    };

    template <typename V, typename Int>
    requires concepts::droppable_last_range<V, Int>
    struct drop_last_view : std::ranges::view_interface<drop_last_view<V, Int>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(drop_last_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(drop_last_iterator, drop_last_sentinel, Int);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        Int drop_n;

        GENEX_INLINE constexpr explicit drop_last_view(V rng, Int drop_n) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)), drop_n(std::max(0, drop_n)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::begin(base_rng)) and noexcept(iterators::end(base_rng))) {
            const auto dist = iterators::distance(base_rng);
            const auto cut = dist > drop_n ? dist - drop_n : 0;
            return iterators::next(iterators::begin(base_rng), cut, iterators::end(base_rng));
        }
    };
}


namespace genex::views {
    struct drop_last_fn {
        template <typename Rng, typename Int>
        requires detail::concepts::droppable_last_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int drop_n) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::drop_last_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), drop_n};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_last_range<Rng, Int> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int drop_n) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::drop_last_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), drop_n}; // .as_pointer_subrange();
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int drop_n) const -> auto {
            return std::bind_back(
                drop_last_fn{}, drop_n);
        }
    };

    inline constexpr drop_last_fn drop_last{};
}
