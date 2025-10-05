#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>

#include "genex/iterators/next.hpp"


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept chunkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept chunkable_range =
        forward_range<Rng> and
        chunkable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::chunkable_iters<I, S>
    struct chunk_iterator {
        using reference = iter_reference_t<I>;
        using value_type = std::ranges::subrange<I, S>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(chunk_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(chunk_iterator);

        std::size_t chunk_size;
        I chunk_end;

        GENEX_INLINE constexpr explicit chunk_iterator(I it, S st, const std::size_t chunk_size) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)), chunk_size(chunk_size) {
            chunk_end = iterators::next(this->it, this->chunk_size, this->st);
        }

        GENEX_INLINE constexpr auto operator*() const noexcept
            -> value_type {
            return {it, chunk_end};
        }

        GENEX_INLINE constexpr auto operator->() const noexcept
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> chunk_iterator& {
            if (it != st) {
                it = chunk_end;
                chunk_end = iterators::next(it, chunk_size, st);
            }
            return *this;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> chunk_iterator& requires std::random_access_iterator<I> {
            if (it != st) {
                it = chunk_end;
                const auto remaining = static_cast<std::size_t>(iterators::distance(it, st));
                const auto step = std::min(chunk_size, remaining);
                chunk_end = it + static_cast<difference_type>(step);;
            }
            return *this;
        }
    };

    template <typename S>
    struct chunk_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(chunk_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(chunk_iterator, chunk_sentinel);
    };

    template <typename V>
    requires (concepts::chunkable_range<V>)
    struct chunk_view : std::ranges::view_interface<chunk_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(chunk_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(chunk_iterator, chunk_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(chunk_size);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        std::size_t chunk_size;

        GENEX_INLINE constexpr explicit chunk_view(V rng, const std::size_t chunk_size) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)), chunk_size(chunk_size) {
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
    /**
     * The @c chunk view adaptor splits a range into subranges of a specified size. Each subrange is represented as a
     * @c std::ranges::subrange.
     */
    struct chunk_fn {
        template <typename Rng>
        requires detail::concepts::chunkable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const std::size_t chunk_size) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::chunk_view<V>{
                std::views::all(std::forward<Rng>(rng)), chunk_size};
        }

        template <typename Rng>
        requires detail::concepts::chunkable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, std::size_t chunk_size) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::chunk_view<V>{
                std::views::all(std::forward<Rng>(rng)), chunk_size}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()(const std::size_t chunk_size) const -> auto {
            return std::bind_back(
                chunk_fn{}, chunk_size);
        }
    };

    inline constexpr chunk_fn chunk{};
}
