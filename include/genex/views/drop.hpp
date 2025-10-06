#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::movable<I>;

    template <typename Rng, typename Int>
    concept droppable_range =
        input_range<Rng> and
        droppable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
    requires concepts::droppable_iters<I, S, Int>
    struct drop_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_INLINE constexpr explicit drop_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            drop_iterator, it);

        GENEX_INLINE constexpr explicit drop_iterator(I it, S st, Int) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> drop_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
        noexcept(it++)) -> drop_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct drop_sentinel { };

    template <typename I, typename S, typename Int>
    requires concepts::droppable_iters<I, S, Int>
    drop_iterator(I it, S st, Int drop_n) -> drop_iterator<I, S, Int>;

    template <typename I, typename S, typename Int>
    requires concepts::droppable_iters<I, S, Int>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(drop_iterator, drop_sentinel, I, S, Int) {
        return it.it == it.st;
    }

    template <typename V, typename Int>
    requires concepts::droppable_range<V, Int>
    struct drop_view : std::ranges::view_interface<drop_view<V, Int>> {
        V base_rng;
        Int drop_n;

        GENEX_INLINE constexpr explicit drop_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            drop_iterator, drop_sentinel, base_rng, drop_n);

        GENEX_INLINE constexpr explicit drop_view(V rng, Int drop_n) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<Int>) :
            base_rng(std::move(rng)), drop_n(std::max(0, drop_n)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng)) and
            noexcept(iterators::end(base_rng))) {
            return iterators::next(iterators::begin(base_rng), drop_n, iterators::end(base_rng));
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }

        GENEX_INLINE constexpr auto size() const noexcept(
            noexcept(operations::size(base_rng))) -> range_size_t<V> {
            const auto total_size = operations::size(base_rng);
            return total_size > drop_n ? total_size - drop_n : 0;
        }
    };
}


namespace genex::views {
    struct drop_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I it, S st, Int drop_n) const noexcept -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::drop_view<V, Int>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}, drop_n};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int drop_n) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::drop_view<V, Int>{
                std::forward<Rng>(rng), drop_n};
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int drop_n) const noexcept -> auto {
            return std::bind_back(
                drop_fn{}, drop_n);
        }
    };

    inline constexpr drop_fn drop{};
}
