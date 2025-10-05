#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept enumerable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept enumerable_range =
        input_range<Rng> and
        enumerable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::enumerable_iters<I, S>
    struct enum_iterator {
        using value_type = std::pair<std::size_t, iter_value_t<I>>;
        using reference = std::pair<std::size_t, iter_reference_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(enum_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(enum_iterator);

        std::size_t index;

        GENEX_INLINE constexpr explicit enum_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)), index(0) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return {index, *it};
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> enum_iterator& {
            ++it;
            ++index;
            return *this;
        }
    };

    template <typename S>
    struct enum_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(enum_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(enum_iterator, enum_sentinel);
    };

    template <typename V>
    requires concepts::enumerable_range<V>
    struct enum_view : std::ranges::view_interface<enum_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(enum_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(enum_iterator, enum_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit enum_view(V rng) noexcept(
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
    struct enumerate_fn {
        template <typename Rng>
        requires detail::concepts::enumerable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::enum_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        // template <typename Rng>
        // requires detail::concepts::enumerable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        // GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
        //     using V = std::views::all_t<Rng>;
        //     return detail::enum_view<V>{
        //         std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        // }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                enumerate_fn{});
        }
    };

    inline constexpr enumerate_fn enumerate{};
}
