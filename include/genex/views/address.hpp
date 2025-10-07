#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
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
        using reference = std::add_pointer_t<iter_value_t<I>>;
        using value_type = std::add_pointer_t<iter_value_t<I>>;
        using pointer = std::add_pointer_t<iter_value_t<I>>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            address_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            address_iterator, it);

        GENEX_INLINE constexpr explicit address_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(std::addressof(*it))) -> reference {
            return const_cast<reference>(std::addressof(*it));
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> address_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> address_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct address_sentinel { };

    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    address_iterator(I, S) -> address_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(address_iterator, address_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    requires concepts::addressable_range<V>
    struct address_view : std::ranges::view_interface<address_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr address_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            address_iterator, address_sentinel, base_rng);

        GENEX_INLINE constexpr explicit address_view(V rng) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }

        GENEX_INLINE constexpr auto size() const noexcept(
            noexcept(operations::size(base_rng))) {
            return operations::size(base_rng);
        }
    };
}


namespace genex::views {
    struct address_fn {
        template <typename I, typename S>
        requires detail::concepts::addressable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::address_view<V>{
                std::ranges::subrange{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::addressable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::address_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, address_fn>) {
            return std::bind_back(
                address_fn{});
        }
    };

    inline constexpr address_fn address{};
}
