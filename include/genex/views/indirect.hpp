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

        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_INLINE constexpr explicit indirect_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            indirect_iterator, it);

        GENEX_INLINE constexpr explicit indirect_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(**it)) -> reference {
            return **it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> indirect_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
        noexcept(it++)) -> indirect_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct indirect_sentinel { };

    template <typename I, typename S>
    requires concepts::indirectable_iters<I, S>
    indirect_iterator(I, S) -> indirect_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::indirectable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(indirect_iterator, indirect_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    requires concepts::indirectable_range<V>
    struct indirect_view : std::ranges::view_interface<indirect_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit indirect_view() = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            indirect_iterator, indirect_sentinel, base_rng);

        GENEX_INLINE explicit indirect_view(V rng) noexcept(
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
            noexcept(operations::size(base_rng))) -> range_size_t<V> {
            return operations::size(base_rng);
        }
    };
}


namespace genex::views {
    struct indirect_fn {
        template <typename I, typename S>
        requires detail::concepts::indirectable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::indirect_view<V>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::indirectable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::indirect_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                indirect_fn{});
        }
    };

    inline constexpr indirect_fn indirect{};
}
