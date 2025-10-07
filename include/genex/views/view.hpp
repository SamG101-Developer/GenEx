#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept viewable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept viewable_range =
        input_range<Rng> and
        viewable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::viewable_iters<I, S>
    struct view_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            view_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            view_iterator, it);

        GENEX_INLINE constexpr explicit view_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> view_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> view_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct view_sentinel { };

    template <typename I, typename S>
    requires concepts::viewable_iters<I, S>
    view_iterator(I, S) -> view_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::viewable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(view_iterator, view_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    struct view_view : std::ranges::view_interface<view_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit view_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            view_iterator, view_sentinel, base_rng);

        GENEX_INLINE constexpr explicit view_view(V rng) noexcept(
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
    struct view_fn {
        template <typename I, typename S>
        requires detail::concepts::viewable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::view_view<V>{
                std::ranges::subrange{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::viewable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::view_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, view_fn>) {
            return std::bind_back(
                view_fn{});
        }
    };

    inline constexpr view_fn view{};
}
