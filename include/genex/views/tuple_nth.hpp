#pragma once
#include <iterator>
#include <utility>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, std::size_t N>
    concept tuple_nth_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        tuple_like<iter_value_t<I>> and
        N <= std::tuple_size_v<iter_value_t<I>>;

    template <typename Rng, std::size_t N>
    concept tuple_nth_range =
        std::ranges::input_range<Rng> and
        tuple_nth_iters<iterator_t<Rng>, sentinel_t<Rng>, N>;
}


namespace genex::views::detail {
    template <typename I, typename S, std::size_t N>
    requires concepts::tuple_nth_iters<I, S, N>
    struct tuple_n_iterator {
        using reference = std::tuple_element_t<N, iter_value_t<I>>&;
        using value_type = std::remove_cvref_t<reference>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(tuple_n_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(tuple_n_iterator);

        GENEX_INLINE constexpr explicit tuple_n_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(std::get<N>(*it)))
            -> reference {
            return std::get<N>(*it);
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(std::get<N>(*it))))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> tuple_n_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, std::size_t N>
    struct tuple_n_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(tuple_n_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(tuple_n_iterator, tuple_n_sentinel, N);
    };

    template <typename V, std::size_t N>
    requires concepts::tuple_nth_range<V, N>
    struct tuple_n_view : std::ranges::view_interface<tuple_n_view<V, N>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(tuple_n_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(tuple_n_iterator, tuple_n_sentinel, N);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit tuple_n_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    struct tuple_nth_fn {
        template <std::size_t N, typename Rng>
        requires detail::concepts::tuple_nth_range<Rng, N>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::tuple_n_view<V, N>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <std::size_t N, typename Rng>
        requires detail::concepts::tuple_nth_range<Rng, N> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::tuple_n_view<V, N>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        template <std::size_t N>
        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return [*this]<typename Rng> requires detail::concepts::tuple_nth_range<Rng, N>(Rng &&rng) {
                return this->operator()<N>(std::forward<Rng>(rng));
            };
        }
    };

    inline constexpr tuple_nth_fn tuple_nth_{};
}


#define tuple_nth tuple_nth_.operator()
