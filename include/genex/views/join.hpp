#pragma once
#include <utility>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept joinable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::movable<I>;

    template <typename Rng>
    concept joinable_range =
        input_range<Rng> and
        joinable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
    struct join_iterator {
        using reference = range_reference_t<iter_value_t<I>>;
        using value_type = range_value_t<iter_value_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category); // I? or inner iterator?
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(join_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(join_iterator);

        iterator_t<iter_value_t<I>> sub_it;
        iterator_t<iter_value_t<I>> sub_st;

        GENEX_INLINE constexpr explicit join_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
            if (it != st) {
                sub_it = iterators::begin(*it);
                sub_st = iterators::end(*it);
            }
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*sub_it))
            -> value_type {
            return *sub_it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*sub_it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and noexcept(++sub_it)) // begin & end ?
            -> join_iterator& {
            ++sub_it;
            if (sub_it == sub_st and it != st) {
                ++it;
                sub_it = iterators::begin(*it);
                sub_st = iterators::end(*it);
            }
            return *this;
        }
    };

    template <typename S>
    struct join_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(join_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(join_iterator, join_sentinel);
    };

    template <typename V>
    requires concepts::joinable_range<V>
    struct join_view : std::ranges::view_interface<join_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(join_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(join_iterator, join_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit join_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    struct join_fn {
        template <typename Rng>
        requires detail::concepts::joinable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng) const -> auto {
            return detail::join_view{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::joinable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng) const -> auto {
            return detail::join_view{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                join_fn{});
        }
    };

    constexpr inline join_fn join{};
}
