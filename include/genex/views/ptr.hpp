#pragma once

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept ptr_gettable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename Rng>
    concept ptr_gettable_range =
        input_range<Rng> and
        ptr_gettable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::ptr_gettable_iters<I, S>
    struct ptr_iterator {
        using reference = std::remove_reference_t<decltype(*std::declval<I>()->get())>&;
        using value_type = std::add_pointer_t<std::remove_reference_t<reference>>;
        using pointer = value_type;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(ptr_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(ptr_iterator);

        GENEX_INLINE constexpr explicit ptr_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it->get()))
            -> value_type {
            return it->get();
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it->get())))
            -> pointer {
            return it->get();
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> ptr_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S>
    struct ptr_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(ptr_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(ptr_iterator, ptr_sentinel);
    };

    template <typename V>
    requires concepts::ptr_gettable_range<V>
    struct ptr_view : std::ranges::view_interface<ptr_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(ptr_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(ptr_iterator, ptr_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit ptr_view(V rng) noexcept(
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
    struct ptr_fn {
        template <typename Rng>
        requires detail::concepts::ptr_gettable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::ptr_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::ptr_gettable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::ptr_view<V>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                ptr_fn{});
        }
    };

    constexpr inline ptr_fn ptr{};
}
