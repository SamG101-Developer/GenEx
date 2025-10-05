#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/algorithms/find.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename E>
    concept splittable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, I, std::remove_cvref_t<E> const*> and
        std::copyable<E> and
        std::movable<I>;

    template <typename Rng, typename E>
    concept splittable_range =
        forward_range<Rng> and
        splittable_iters<iterator_t<Rng>, sentinel_t<Rng>, E>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename E>
    requires concepts::splittable_iters<I, S, E>
    struct split_iterator {
        using reference = iter_reference_t<I>;
        using value_type = std::ranges::subrange<I, S>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(split_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(split_iterator);

        E val;
        I chunk_end;

        GENEX_INLINE constexpr explicit split_iterator(I it, S st, E val) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<E>) :
            it(std::move(it)), st(std::move(st)), val(std::move(val)) {
            chunk_end = algorithms::find(this->it, this->st, this->val);
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> value_type {
            return {it, chunk_end};
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it)) // next & find?
            -> split_iterator& {
            if (it == st) { return *this; }
            if (chunk_end != st) {
                it = st;
                return *this;
            }

            it = iterators::next(chunk_end);
            chunk_end = algorithms::find(it, st, val);
            return *this;
        }
    };

    template <typename S, typename E>
    struct split_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(split_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(split_iterator, split_sentinel, E);
    };

    template <typename V, typename E>
    requires (concepts::splittable_range<V, E>)
    struct split_view : std::ranges::view_interface<split_view<V, E>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(split_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(split_iterator, split_sentinel, E);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(val);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        E val;

        GENEX_INLINE constexpr explicit split_view(V rng, E val) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<E>) :
            base_rng(std::move(rng)), val(std::move(val)) {
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
    struct split_fn {
        template <typename E, typename Rng>
        requires detail::concepts::splittable_range<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E val) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::split_view<V, E>{
                std::views::all(std::forward<Rng>(rng)), std::move(val)};
        }

        template <typename E, typename Rng>
        requires detail::concepts::splittable_range<Rng, E> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E val) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::split_view<V, E>{
                std::views::all(std::forward<Rng>(rng)), std::move(val)}; // .as_pointer_subrange();
        }

        template <typename E>
        GENEX_INLINE constexpr auto operator()(E val) const -> auto {
            return std::bind_back(
                split_fn{}, std::move(val));
        }
    };

    inline constexpr split_fn split{};
}
