#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
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

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        std::size_t index = 0;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            enum_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            enum_iterator, it);

        GENEX_INLINE constexpr explicit enum_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)), index(0) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return {index, *it};
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> enum_iterator& {
            ++it;
            ++index;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> enum_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct enum_sentinel { };

    template <typename I, typename S>
    requires concepts::enumerable_iters<I, S>
    enum_iterator(I, S) -> enum_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::enumerable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(enum_iterator, enum_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    requires concepts::enumerable_range<V>
    struct enum_view : std::ranges::view_interface<enum_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit enum_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            enum_iterator, enum_sentinel, base_rng);

        GENEX_INLINE constexpr explicit enum_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
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
    struct enumerate_fn {
        template <typename I, typename S>
        requires detail::concepts::enumerable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::enum_view<V>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::enumerable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::enum_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, enumerate_fn>) {
            return std::bind_back(
                enumerate_fn{});
        }
    };

    inline constexpr enumerate_fn enumerate{};
}
