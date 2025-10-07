#pragma once
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/next.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept takeable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::movable<I>;

    template <typename Rng, typename Int>
    concept takeable_last_range =
        input_range<Rng> and
        takeable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
    requires concepts::takeable_last_iters<I, S, Int>
    struct take_last_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            take_last_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            take_last_iterator, it);

        GENEX_INLINE constexpr explicit take_last_iterator(I it, S st, Int) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Int>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> take_last_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> take_last_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct take_last_sentinel { };

    template <typename I, typename S, typename Int>
    requires concepts::takeable_last_iters<I, S, Int>
    take_last_iterator(I, S, Int) -> take_last_iterator<I, S, Int>;

    template <typename I, typename S, typename Int>
    requires concepts::takeable_last_iters<I, S, Int>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(take_last_iterator, take_last_sentinel, I, S, Int) {
        return it.it == it.st;
    }

    template <typename V, typename Int>
    requires concepts::takeable_last_range<V, Int>
    struct take_last_view : std::ranges::view_interface<take_last_view<V, Int>> {
        V base_rng;
        Int take_n;

        GENEX_INLINE constexpr explicit take_last_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            take_last_iterator, take_last_sentinel, base_rng, take_n);

        GENEX_INLINE constexpr explicit take_last_view(V rng, Int take_n) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, V, Int>) :
            base_rng(std::move(rng)), take_n(std::move(take_n)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng))) {
            const auto dist = iterators::distance(base_rng);
            const auto cut = dist > take_n ? dist - take_n : 0;
            return iterators::next(iterators::begin(base_rng), cut, iterators::end(base_rng));
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }

        GENEX_INLINE constexpr auto size() const noexcept(
            noexcept(operations::size(base_rng))) {
            const auto total_size = operations::size(base_rng);
            return total_size > take_n ? take_n : total_size;
        }
    };
}


namespace genex::views {
    struct take_last_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_last_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I it, S st, Int n) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Int>) {
            using V = std::ranges::subrange<I, S>;
            return detail::take_last_view<V, Int>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}, std::move(n)};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_last_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int n) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&> and
            meta::all_of_v<std::is_nothrow_move_constructible, Int>) {
            using V = std::views::all_t<Rng>;
            return detail::take_last_view<V, Int>{
                std::forward<Rng>(rng), std::move(n)};
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int n) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, Int>) {
            return std::bind_back(
                take_last_fn{}, std::move(n));
        }
    };

    inline constexpr take_last_fn take_last{};
}
