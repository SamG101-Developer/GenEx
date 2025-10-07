#pragma once

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept cyclable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept cyclable_range =
        forward_range<Rng> and
        cyclable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::cyclable_iters<I, S>
    struct cycle_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        I copy_it;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            cycle_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            cycle_iterator, it);

        GENEX_INLINE constexpr explicit cycle_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)) {
            copy_it = this->it;
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it) and noexcept(it == st)) -> cycle_iterator& {
            ++it;
            if (it == st) { it = copy_it; }
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> cycle_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct cycle_sentinel { };

    template <typename I, typename S>
    requires concepts::cyclable_iters<I, S>
    cycle_iterator(I, S) -> cycle_iterator<I, S>;

    template <typename I, typename S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(cycle_iterator, cycle_sentinel, I, S) {
        return false;
    }

    template <typename V>
    requires (concepts::cyclable_range<V>)
    struct cycle_view : std::ranges::view_interface<cycle_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit cycle_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            cycle_iterator, cycle_sentinel, base_rng)

        GENEX_INLINE constexpr explicit cycle_view(V rng) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept (
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }

        GENEX_INLINE constexpr auto size() const noexcept = delete;
    };
}


namespace genex::views {
    struct cycle_fn {
        template <typename I, typename S>
        requires detail::concepts::cyclable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::cycle_view<V>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::cyclable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::cycle_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, cycle_fn>) {
            return std::bind_front(cycle_fn{});
        }
    };

    inline constexpr cycle_fn cycle{};
}
