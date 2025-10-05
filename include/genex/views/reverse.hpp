#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/prev.hpp>
#include <genex/operations/data.hpp>
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

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::bidirectional_iterator_tag);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(reverse_iterator);

        GENEX_INLINE constexpr explicit reverse_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(--it))
            -> reverse_iterator& {
            --it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(noexcept(++it))
            -> reverse_iterator requires std::forward_iterator<I> {
            auto tmp = *this;
            --it;
            return tmp;
        }

        GENEX_INLINE constexpr auto operator--() noexcept(noexcept(++it))
            -> reverse_iterator& requires std::bidirectional_iterator<I> {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator--(int) noexcept(noexcept(--it))
            -> reverse_iterator requires std::bidirectional_iterator<I> {
            auto tmp = *this;
            --it;
            return tmp;
        }

        GENEX_INLINE constexpr auto operator+=(difference_type n) noexcept(noexcept(it += n))
            -> reverse_iterator& requires std::random_access_iterator<I> {
            it -= n;
            return *this;
        }

        GENEX_INLINE constexpr auto operator+(difference_type n) const noexcept(noexcept(it + n))
            -> reverse_iterator requires std::random_access_iterator<I> {
            return reverse_iterator{it - n};
        }

        GENEX_INLINE constexpr auto operator-=(difference_type n) noexcept(noexcept(it -= n))
            -> reverse_iterator& requires std::random_access_iterator<I> {
            it += n; return *this;
        }

        GENEX_INLINE constexpr auto operator-(difference_type n) const noexcept(noexcept(it - n))
            -> reverse_iterator requires std::random_access_iterator<I> {
            return reverse_iterator{it + n};
        }

        GENEX_INLINE constexpr auto operator==(reverse_iterator const &other) const noexcept(noexcept(it == other.it))
            -> bool {
            return it == other.it;
        }

        GENEX_INLINE constexpr auto operator!=(reverse_iterator const &other) const noexcept(noexcept(it != other.it))
            -> bool {
            return it != other.it;
        }
    };

    template <typename S>
    struct reverse_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(reverse_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(reverse_iterator, reverse_sentinel);
    };

    template <typename V>
    requires concepts::reversible_iters_range<V>
    struct reverse_view : std::ranges::view_interface<reverse_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(reverse_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(reverse_iterator, reverse_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit reverse_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::prev(iterators::end(base_rng)))) {
            return iterators::end(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::prev(iterators::begin(base_rng)))) {
            return iterators::begin(base_rng);
        }
    };
}


namespace genex::views {
    struct reverse_fn {
        template <typename Rng>
        requires detail::concepts::reversible_iters_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            return detail::reverse_view{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::reversible_iters_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            return detail::reverse_view{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                reverse_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(reverse);
}
