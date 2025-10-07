#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename To>
    concept static_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        requires(iter_value_t<I> &&from) { static_cast<To>(from); };

    template <typename Rng, typename To>
    concept static_castable_range =
        input_range<Rng> and
        static_castable_iters<iterator_t<Rng>, sentinel_t<Rng>, To>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename To>
    requires concepts::static_castable_iters<I, S, To>
    struct cast_static_iterator {
        using reference = std::add_lvalue_reference_t<To>;
        using value_type = To;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_INLINE constexpr explicit cast_static_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            cast_static_iterator, it);

        GENEX_INLINE constexpr explicit cast_static_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr cast_static_iterator(cast_static_iterator &&other) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(other.it)), st(std::move(other.st)) {
        }

        GENEX_INLINE constexpr auto operator=(cast_static_iterator &&other) noexcept(
            meta::all_of_v<std::is_nothrow_move_assignable, I, S>) -> cast_static_iterator& {
            it = std::move(other.it);
            st = std::move(other.st);
            return *this;
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(static_cast<To>(*it))) -> value_type {
            return static_cast<To>(*it);
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> cast_static_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> cast_static_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct cast_static_sentinel { };

    template <typename I, typename S, typename To>
    requires concepts::static_castable_iters<I, S, To>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(cast_static_iterator, cast_static_sentinel, I, S, To) {
        return it.it == it.st;
    }

    template <typename V, typename To>
    requires concepts::static_castable_range<V, To>
    struct cast_static_view : std::ranges::view_interface<cast_static_view<V, To>> {
        V base_rng;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            cast_static_iterator<iterator_t<V> COMMA sentinel_t<V> COMMA To>, cast_static_sentinel, base_rng);

        GENEX_INLINE constexpr explicit cast_static_view(V rng) noexcept(
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

        GENEX_INLINE constexpr auto size() const noexcept(
            noexcept(operations::size(base_rng))) {
            return operations::size(base_rng);
        }
    };
}


namespace genex::views {
    struct cast_static_fn {
        template <typename To, typename I, typename S>
        requires detail::concepts::static_castable_iters<I, S, To>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::cast_static_view<V, To>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}};
        }

        template <typename To, typename Rng>
        requires detail::concepts::static_castable_range<Rng, To>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::cast_static_view<V, To>{
                std::forward<Rng>(rng)};
        }

        template <typename To>
        GENEX_INLINE constexpr auto operator()() const noexcept {
            return [*this]<typename Rng>(Rng &&rng) noexcept(meta::all_of_v<std::is_nothrow_constructible, Rng&&>)
            requires detail::concepts::static_castable_range<Rng, To> {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    inline constexpr cast_static_fn cast_static_{};
}


#define cast_static cast_static_.operator()
