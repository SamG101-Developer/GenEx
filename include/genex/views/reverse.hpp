#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept reversible_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept reversible_iters_range =
        bidirectional_range<Rng> and
        reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
    struct reverse_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            reverse_iterator);

        GENEX_INLINE constexpr explicit reverse_iterator(I it, S st) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            auto tmp = it;
            --tmp;
            return *tmp;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(--it)) -> reverse_iterator& {
            --it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(--it)) -> reverse_iterator {
            auto tmp = *this;
            --it;
            return tmp;
        }

        GENEX_INLINE constexpr auto operator--() noexcept(
            noexcept(++it)) -> reverse_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator--(int) noexcept(
            noexcept(++it)) -> reverse_iterator {
            auto tmp = *this;
            ++it;
            return tmp;
        }

        GENEX_INLINE constexpr auto operator+=(difference_type n) noexcept(
            noexcept(it -= n)) -> reverse_iterator& requires std::random_access_iterator<I> {
            it -= n;
            return *this;
        }

        GENEX_INLINE constexpr auto operator+(difference_type n) const noexcept(
            noexcept(it - n)) -> reverse_iterator requires std::random_access_iterator<I> {
            return reverse_iterator{it - n};
        }

        GENEX_INLINE constexpr auto operator-=(difference_type n) noexcept(
            noexcept(it += n)) -> reverse_iterator& requires std::random_access_iterator<I> {
            it += n; return *this;
        }

        GENEX_INLINE constexpr auto operator-(difference_type n) const noexcept(
            noexcept(it + n)) -> reverse_iterator requires std::random_access_iterator<I> {
            return reverse_iterator{it + n};
        }

        GENEX_INLINE constexpr auto operator==(reverse_iterator const &other) const noexcept(
            noexcept(it == other.it)) -> bool {
            return it == other.it;
        }

        GENEX_INLINE constexpr auto operator!=(reverse_iterator const &other) const noexcept(
            noexcept(it != other.it)) -> bool {
            return it != other.it;
        }
    };

    struct reverse_sentinel { };

    template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
    reverse_iterator(I, S) -> reverse_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(reverse_iterator, reverse_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    requires concepts::reversible_iters_range<V>
    struct reverse_view : std::ranges::view_interface<reverse_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit reverse_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            reverse_iterator, reverse_sentinel, base_rng)

        GENEX_INLINE constexpr explicit reverse_view(V rng) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto size() const noexcept(
            noexcept(operations::size(base_rng))) {
            return operations::size(base_rng);
        }
    };
}


namespace genex::views {
    struct reverse_fn {
        template <typename I, typename S>
        requires detail::concepts::reversible_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S>) {
            using V = std::ranges::subrange<I, S>;
            return detail::reverse_view<V>{
                std::ranges::subrange{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::reversible_iters_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            using V = std::views::all_t<Rng>;
            return detail::reverse_view<V>{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, reverse_fn>) {
            return std::bind_back(
                reverse_fn{});
        }
    };

    inline constexpr reverse_fn reverse{};
}
