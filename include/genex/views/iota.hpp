#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail {
    template <typename Int>
    requires std::weakly_incrementable<Int>
    struct iota_iterator {
        using I = Int;
        using S = Int;
        
        using reference = std::add_lvalue_reference_t<Int>;
        using value_type = Int;
        using pointer = std::add_pointer_t<Int>;

        using iterator_category = std::conditional_t<
            advancable<Int>, std::random_access_iterator_tag, std::conditional_t<
            decrementable<Int>, std::bidirectional_iterator_tag, std::conditional_t<
            incrementable<Int>, std::forward_iterator_tag, std::input_iterator_tag>>>;
        using iterator_concept = iterator_category;
        using difference_type = std::make_signed_t<Int>;

        I it; S st;
        Int step;

        GENEX_INLINE constexpr explicit iota_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            iota_iterator, it);

        GENEX_INLINE constexpr explicit iota_iterator(I it, S st, Int step) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<Int>) :
            it(std::move(it)), st(std::move(st)), step(step) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept
            -> value_type {
            return it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept
            -> iota_iterator& {
            if ((step > 0 and it + step > st) or (step < 0 and it + step < st)) { it = st; }
            else { it += step; }
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> iota_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct iota_sentinel { };

    template <typename Int>
    requires std::weakly_incrementable<Int>
    iota_iterator(Int, Int, Int) -> iota_iterator<Int>;

    template <typename Int>
    requires std::weakly_incrementable<Int>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(iota_iterator, iota_sentinel, Int) {
        return it.it == it.st;
    }

    template <typename Int>
    requires std::weakly_incrementable<Int>
    struct iota_view : std::ranges::view_interface<iota_view<Int>> {
        Int from;
        Int to;
        Int step;

        GENEX_INLINE constexpr explicit iota_view() noexcept = default;

        GENEX_INLINE constexpr explicit iota_view(Int from, Int to, Int step) noexcept(
            std::is_nothrow_move_constructible_v<Int>) :
            from(from), to(to), step(step) {
        }
        
        GENEX_INLINE constexpr auto begin() const noexcept(
            noexcept(internal_begin()) and noexcept(internal_end())) {
            return iota_iterator{internal_begin(), internal_end(), step};
        }

        GENEX_INLINE constexpr auto end() const noexcept(
            noexcept(internal_end())) {
            return iota_sentinel{};
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            std::is_nothrow_copy_constructible_v<Int>){
            return from;
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            std::is_nothrow_copy_constructible_v<Int>){
            return to;
        }

        GENEX_INLINE constexpr auto size() const -> Int
            requires sized_range<iota_view> {
            return (to - from) / step + ((to - from) % step != 0 ? 1 : 0);
        }
    };
}


namespace genex::views {
    struct iota_fn {
        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int from, Int to, Int step = static_cast<Int>(1)) const noexcept -> auto {
            return detail::iota_view<Int>{
                from, to, step};
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int to) const noexcept -> auto {
            return detail::iota_view<Int>{
                static_cast<Int>(0), to, static_cast<Int>(1)};
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return detail::iota_view<Int>{
                static_cast<Int>(0), std::numeric_limits<Int>::max(), static_cast<Int>(1)};
        }
    };

    constexpr inline iota_fn iota{}; 
}
