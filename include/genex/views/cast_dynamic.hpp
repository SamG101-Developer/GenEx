#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename To>
    concept dynamic_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        (std::is_pointer_v<To> or std::is_reference_v<To>) and
        requires(iter_value_t<I> &&from) { dynamic_cast<To>(from); };

    template <typename Rng, typename To>
    concept dynamic_castable_range =
        input_range<Rng> and
        dynamic_castable_iters<iterator_t<Rng>, sentinel_t<Rng>, To>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename To>
    requires concepts::dynamic_castable_iters<I, S, To>
    struct cast_dynamic_iterator {
        using reference = std::add_lvalue_reference_t<To>;
        using value_type = To;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_INLINE constexpr explicit cast_dynamic_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            cast_dynamic_iterator, it);

        GENEX_INLINE constexpr explicit cast_dynamic_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(dynamic_cast<To>(*it))) -> value_type {
            return dynamic_cast<To>(*it);
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> cast_dynamic_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
        noexcept(it++)) -> cast_dynamic_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct cast_dynamic_sentinel { };

    template <typename I, typename S, typename To>
    requires concepts::dynamic_castable_iters<I, S, To>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(cast_dynamic_iterator, cast_dynamic_sentinel, I, S, To) {
        return it.it == it.st;
    }

    template <typename V, typename To>
    requires concepts::dynamic_castable_range<V, To>
    struct cast_dynamic_view : std::ranges::view_interface<cast_dynamic_view<V, To>> {
        V base_rng;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            cast_dynamic_iterator<iterator_t<V> COMMA sentinel_t<V> COMMA To>, cast_dynamic_sentinel, base_rng);

        GENEX_INLINE constexpr explicit cast_dynamic_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
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
    };
}


namespace genex::views {
    struct cast_dynamic_fn {
        template <typename To, typename I, typename S>
        requires detail::concepts::dynamic_castable_iters<I, S, To>
        GENEX_INLINE constexpr auto operator()(I it, S st) const -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::cast_dynamic_view<V, To>{
                std::ranges::subrange(it, st)};
        }

        template <typename To, typename Rng>
        requires detail::concepts::dynamic_castable_range<Rng, To>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::cast_dynamic_view<V, To>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename To, typename Rng>
        requires detail::concepts::dynamic_castable_range<Rng, To> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::cast_dynamic_view<V, To>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        template <typename To>
        requires std::is_pointer_v<To> or std::is_reference_v<To>
        GENEX_INLINE constexpr auto operator()() const -> auto {
            return [*this]<typename Rng>(Rng &&rng) requires detail::concepts::dynamic_castable_range<Rng, To> {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    inline constexpr cast_dynamic_fn cast_dynamic_{};
}


#define cast_dynamic cast_dynamic_.operator()

