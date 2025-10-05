#pragma once
#include <functional>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept keyable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        pair_like<iter_reference_t<I>> and
        std::movable<I>;

    template <typename I, typename S>
    concept valable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        pair_like<iter_reference_t<I>> and
        std::movable<I>;

    template <typename Rng>
    concept keyable_range =
        input_range<Rng> and
        keyable_iters<iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename Rng>
    concept valable_range =
        input_range<Rng> and
        valable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::keyable_iters<I, S>
    struct keys_iterator {
        using reference = std::tuple_element_t<0, iter_value_t<I>>&;
        using value_type = std::tuple_element_t<0, iter_value_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::forward_iterator_tag);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(keys_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(keys_iterator);

        GENEX_INLINE constexpr explicit keys_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(std::get<0>(*it)))
            -> reference {
            return std::get<0>(*it);
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(std::get<0>(*it))))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> keys_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename I, typename S>
    requires concepts::valable_iters<I, S>
    struct vals_iterator {
        using reference = std::tuple_element_t<1, iter_value_t<I>>&;
        using value_type = std::tuple_element_t<1, iter_value_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::forward_iterator_tag);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(vals_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(vals_iterator);

        GENEX_INLINE constexpr explicit vals_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(std::get<1>(*it)))
            -> reference {
            return std::get<1>(*it);
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(std::get<1>(*it))))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> vals_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S>
    struct keys_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(keys_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(keys_iterator, keys_sentinel);
    };

    template <typename S>
    struct vals_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(vals_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(vals_iterator, vals_sentinel);
    };

    template <typename V>
    requires concepts::keyable_range<V>
    struct keys_view : std::ranges::view_interface<keys_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(keys_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(keys_iterator, keys_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit keys_view(V rng) noexcept(
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

    template <typename V>
    requires concepts::valable_range<V>
    struct vals_view : std::ranges::view_interface<vals_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(vals_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(vals_iterator, vals_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit vals_view(V rng) noexcept(
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
    struct keys_fn {
        template <typename Rng>
        requires detail::concepts::keyable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::keys_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::valable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::keys_view<V>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                keys_fn{});
        }
    };

    struct vals_fn {
        template <typename Rng>
        requires detail::concepts::valable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::vals_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::valable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::vals_view<V>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                vals_fn{});
        }
    };

    inline constexpr keys_fn keys{};
    inline constexpr vals_fn vals{};
}
