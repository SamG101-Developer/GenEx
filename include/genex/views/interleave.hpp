#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename T1, typename T2>
    using interleave_common_t = std::common_type_t<
        std::remove_cvref_t<iter_value_t<T1>>,
        std::remove_cvref_t<iter_value_t<T2>>>;

    template <typename T1, typename T2>
    using interleave_common_reference_t = std::common_type_t<
        std::remove_cvref_t<iter_reference_t<T1>>,
        std::remove_cvref_t<iter_reference_t<T2>>>;

    template <typename I1, typename S1, typename I2, typename S2>
    concept interleavable_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::convertible_to<iter_value_t<I1>, interleave_common_t<I1, I2>> and
        std::convertible_to<iter_value_t<I2>, interleave_common_t<I1, I2>> and
        std::movable<I1> and std::movable<S1> and
        std::movable<I2> and std::movable<S2>;

    template <typename Rng1, typename Rng2>
    concept interleavable_range =
        input_range<Rng1> and
        input_range<Rng2> and
        interleavable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename I2, typename S2>
    requires concepts::interleavable_iters<I, S, I2, S2>
    struct interleave_iterator {
        using reference = std::common_reference_t<iter_reference_t<I>, iter_reference_t<I2>>;
        using value_type = std::common_type_t<iter_value_t<I>, iter_value_t<I2>>;
        using pointer = std::add_pointer_t<value_type>;
        using iterator_concept = common_iterator_category_t<
            typename std::iterator_traits<I>::iterator_category,
            typename std::iterator_traits<I2>::iterator_category>;
        using iterator_category = iterator_concept;
        using difference_type = difference_type_selector_t<I>;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(interleave_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(interleave_iterator);

        I2 it2;
        S2 st2;
        bool exhaust;

        bool turn = true;

        GENEX_INLINE constexpr explicit interleave_iterator(I it1, S st1, I2 it2, S2 st2, const bool exhaust) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<I2> and
            std::is_nothrow_move_constructible_v<S2>) :
            it(std::move(it1)), st(std::move(st1)), it2(std::move(it2)), st2(std::move(st2)), exhaust(exhaust) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it) and noexcept(*it2))
            -> reference {
            return turn ? *it : *it2;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)) and noexcept(std::addressof(*it2)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and noexcept(++it2))
            -> interleave_iterator& {
            if (turn) {
                if (it != st) { ++it; }
            }
            else {
                if (it2 != st2) { ++it2; }
            }
            turn = not turn;
            return *this;
        }
    };

    template <typename S, typename I2, typename S2>
    struct interleave_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(interleave_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(interleave_iterator, interleave_sentinel, I2, S2);
    };

    template <typename V, typename V2>
    requires concepts::interleavable_range<V, V2>
    struct interleave_view : std::ranges::view_interface<interleave_view<V, V2>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(interleave_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(interleave_iterator, interleave_sentinel, iterator_t<V2>, sentinel_t<V2>);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(iterators::begin(base_rng2), iterators::end(base_rng2), exhaust);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        V2 base_rng2;
        bool exhaust;

        GENEX_INLINE constexpr explicit interleave_view(V rng1, V2 rng2, const bool exhaust) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<V2>) :
            base_rng(std::move(rng1)), base_rng2(std::move(rng2)), exhaust(exhaust) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng)) and noexcept(iterators::begin(base_rng2))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng)) and noexcept(iterators::end(base_rng2))) {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    struct interleave_fn {
        template <typename I1, typename S1, typename I2, typename S2>
        requires detail::concepts::interleavable_iters<I1, S1, I2, S2>
        GENEX_INLINE constexpr auto operator()(I1 it1, S1 st1, I2 it2, S2 st2, bool exhaust = false) const noexcept -> auto {
            return detail::interleave_view{
                std::ranges::subrange<I1, S1>{std::move(it1), std::move(st1)},
                std::ranges::subrange<I2, S2>{std::move(it2), std::move(st2)},
                exhaust};
        }

        template <typename Rng1, typename Rng2>
        requires detail::concepts::interleavable_range<Rng1, Rng2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, bool exhaust = false) const -> auto {
            return detail::interleave_view{
                std::views::all(std::forward<Rng1>(rng1)), std::views::all(std::forward<Rng2>(rng2)), exhaust};
        }

        template <typename Rng1, typename Rng2>
        requires detail::concepts::interleavable_range<Rng1, Rng2> and contiguous_range<Rng1> and borrowed_range<Rng1> and contiguous_range<Rng2> and borrowed_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, bool exhaust = false) const -> auto {
            return detail::interleave_view{
                std::views::all(std::forward<Rng1>(rng1)), std::views::all(std::forward<Rng2>(rng2)), exhaust}; // .as_pointer_subrange();
        }

        template <typename Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, bool exhaust = false) const noexcept -> auto {
            return std::bind_back(
                interleave_fn{}, std::forward<Rng2>(rng2), exhaust);
        }
    };

    inline constexpr interleave_fn interleave{};
}
