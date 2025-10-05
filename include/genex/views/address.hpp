#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept addressable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        std::movable<I> and
        requires(I it) { { std::addressof(*it) }; };

    template <typename Rng>
    concept addressable_range =
        input_range<Rng> and
        addressable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    struct address_iterator {
        using reference = std::add_pointer_t<std::iter_value_t<I>>;
        using value_type = reference;
        using pointer = reference;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(address_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(address_iterator);

        GENEX_INLINE constexpr explicit address_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(std::addressof(*it)))
            -> reference {
            if constexpr (std::is_pointer_v<I>) {
                return *it;
            }
            else {
                return const_cast<std::remove_const_t<reference>>(std::addressof(*it));
            }
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            return std::addressof(*it);
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> address_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S>
    struct address_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(address_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(address_iterator, address_sentinel);
    };

    template <typename V>
    requires (concepts::addressable_range<V>)
    struct address_view : std::ranges::view_interface<address_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(address_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(address_iterator, address_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit address_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            // if constexpr (std::contiguous_iterator<iterator_t<V>>) {
            //     return std::to_address(iterators::begin(base_rng));
            // }
            // else {
            return iterators::begin(base_rng);
            // }
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            // if constexpr (std::contiguous_iterator<iterator_t<V>>) {
            //     return std::to_address(iterators::end(base_rng));
            // }
            // else {
            return iterators::end(base_rng);
            // }
        }
    };
}


namespace genex::views {
    struct address_fn {
        template <typename Rng>
        requires (detail::concepts::addressable_range<Rng>)
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::address_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::addressable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::address_view<V>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                address_fn{});
        }
    };

    inline constexpr address_fn address{};
}
