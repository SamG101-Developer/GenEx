#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
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
        requires(I it) { { std::addressof(*it) }; };

    template <typename Rng>
    concept addressable_range =
        input_range<Rng> and
        addressable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I>
    requires std::input_iterator<I> and std::indirectly_readable<I>
    struct address_iterator {
        using reference = std::add_pointer_t<std::iter_value_t<I>>;
        using value_type = reference;
        using pointer = reference;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(GENEX_ITER_CONCEPT_FOLLOW_THROUGH);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(address_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(address_iterator);

        constexpr auto operator*() const noexcept
            -> reference {
            return std::addressof(*it);
        }

        constexpr auto operator->() const noexcept
            -> pointer {
            return std::addressof(*it);
        }
    };

    template <typename S>
    struct address_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(address_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(address_iterator, address_sentinel);
    };

    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    struct address_from_iters {
        GENEX_VIEW_ITERSENT_CTOR_DEFINITIONS(address_from_iters);
        GENEX_VIEW_ITERSENT_TYPE_DEFINITIONS(address_iterator, address_sentinel);
        GENEX_VIEW_ITERSENT_FUNC_DEFINITIONS;
    };

    template <typename V>
    requires (concepts::addressable_range<V>) //  and viewable_range<Rng>)
    struct address_view : std::ranges::view_interface<address_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(address_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(address_iterator, address_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS;
    };
}


namespace genex::views {
    struct address_fn {
        template <typename I, typename S>
        requires detail::concepts::addressable_iters<I, S>
        constexpr auto operator()(I first, S last) const noexcept -> auto {
            return detail::address_from_iters<I, S>{std::move(first), std::move(last)};
        }

        template <typename Rng>
        requires (detail::concepts::addressable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::address_view<V>{std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        template <typename Rng>
        requires (detail::concepts::addressable_range<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::address_view<V>{std::views::all(std::forward<Rng>(rng))};
        }

        constexpr auto operator()() const noexcept -> auto {
            return std::bind_front(address_fn{});
        }
    };

    inline constexpr address_fn address{};
}
