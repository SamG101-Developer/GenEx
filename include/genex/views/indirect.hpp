#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept indirectable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        std::movable<I> and
        requires(I it) { { **it }; };

    template <typename Rng>
    concept indirectable_range =
        input_range<Rng> and
        indirectable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::indirectable_iters<I, S>
    struct indirect_iterator {
        using reference = std::remove_reference_t<decltype(**std::declval<I&>())>&;
        using value_type = std::remove_reference_t<std::remove_cv_t<std::remove_reference_t<decltype(**std::declval<I&>())>>>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::forward_iterator_tag);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(indirect_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(indirect_iterator);

        GENEX_INLINE constexpr explicit indirect_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(**it))
            -> reference {
            return **it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(**it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> indirect_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S>
    struct indirect_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(indirect_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(indirect_iterator, indirect_sentinel);
    };

    template <typename V>
    requires concepts::indirectable_range<V>
    struct indirect_view : std::ranges::view_interface<indirect_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(indirect_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(indirect_iterator, indirect_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE explicit indirect_view(V rng) noexcept(
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
    struct indirect_fn {
        template <typename Rng>
        requires detail::concepts::indirectable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::indirect_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::indirectable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::indirect_view<V>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                indirect_fn{});
        }
    };

    inline constexpr indirect_fn indirect{};
}
