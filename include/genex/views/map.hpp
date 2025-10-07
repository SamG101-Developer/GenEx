#pragma once
#include <functional>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
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
        using reference = std::add_lvalue_reference_t<std::tuple_element_t<0, iter_value_t<I>>>;
        using value_type = std::tuple_element_t<0, iter_value_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            keys_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            keys_iterator, it);

        GENEX_INLINE constexpr explicit keys_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(std::get<0>(*it))) -> reference {
            return std::get<0>(*it);
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> keys_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> keys_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    template <typename I, typename S>
    requires concepts::valable_iters<I, S>
    struct vals_iterator {
        using reference = std::add_lvalue_reference_t<std::tuple_element_t<1, iter_value_t<I>>>;
        using value_type = std::tuple_element_t<1, iter_value_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            vals_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            vals_iterator, it);

        GENEX_INLINE constexpr explicit vals_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(std::get<1>(*it))) -> reference {
            return std::get<1>(*it);
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> vals_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> vals_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct keys_sentinel { };

    struct vals_sentinel { };

    template <typename I, typename S>
    requires concepts::keyable_iters<I, S>
    keys_iterator(I, S) -> keys_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::valable_iters<I, S>
    vals_iterator(I, S) -> vals_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::keyable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(keys_iterator, keys_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename I, typename S>
    requires concepts::valable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(vals_iterator, vals_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    requires concepts::keyable_range<V>
    struct keys_view : std::ranges::view_interface<keys_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit keys_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            keys_iterator, keys_sentinel, base_rng);

        GENEX_INLINE constexpr explicit keys_view(V rng) noexcept(
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

    template <typename V>
    requires concepts::valable_range<V>
    struct vals_view : std::ranges::view_interface<vals_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit vals_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            vals_iterator, vals_sentinel, base_rng);

        GENEX_INLINE constexpr explicit vals_view(V rng) noexcept(
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
    struct keys_fn {
        template <typename I, typename S>
        requires detail::concepts::keyable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::keys_view<V>{
                std::ranges::subrange{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::keyable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::keys_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, keys_fn>) {
            return std::bind_back(
                keys_fn{});
        }
    };

    struct vals_fn {
        template <typename I, typename S>
        requires detail::concepts::valable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::vals_view<V>{
                std::ranges::subrange{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::valable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::vals_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, vals_fn>) {
            return std::bind_back(
                vals_fn{});
        }
    };

    inline constexpr keys_fn keys{};
    inline constexpr vals_fn vals{};
}
