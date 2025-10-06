#pragma once
#include <utility>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept movable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>> and
        std::movable<I>;

    template <typename Rng>
    concept movable_range =
        input_range<Rng> and
        movable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::movable_iters<I, S>
    struct move_iterator {
        using reference = iter_value_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_INLINE constexpr explicit move_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            move_iterator, it);

        GENEX_INLINE constexpr explicit move_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> value_type {
            return std::move(*it);
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> move_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> move_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct move_sentinel { };

    template <typename I, typename S>
    requires concepts::movable_iters<I, S>
    move_iterator(I, S) -> move_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::movable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(move_iterator, move_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    requires concepts::movable_range<V>
    struct move_view : std::ranges::view_interface<move_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit move_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            move_iterator, move_sentinel, base_rng);

        GENEX_INLINE constexpr explicit move_view(V rng) noexcept(
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
    };
}


namespace genex::views {
    struct move_fn {
        template <typename I, typename S>
        requires detail::concepts::movable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::move_view<V>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::movable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::move_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::movable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::move_view<V>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                move_fn{});
        }
    };

    inline constexpr move_fn move{};
}
