#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept removable_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::movable<I>;

    template <typename Rng, typename Pred, typename Proj>
    concept removable_if_range =
        input_range<Rng> and
        removable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::removable_if_iters<I, S, Pred, Proj>
    struct remove_if_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(remove_if_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(remove_if_iterator);

        Pred pred;
        Proj proj;

        GENEX_INLINE constexpr explicit remove_if_iterator(I it, S st, Pred pred, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<Pred> and
            std::is_nothrow_move_constructible_v<Proj>) :
            it(std::move(it)), st(std::move(st)), pred(std::move(pred)), proj(std::move(proj)) {
            while (it != st and std::invoke(pred, std::invoke(proj, *it))) { ++it; }
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and std::is_nothrow_invocable_v<Pred, std::invoke_result_t<Proj, iter_reference_t<I>>>)
            -> remove_if_iterator& {
            ++it;
            while (it != st and std::invoke(pred, std::invoke(proj, *it))) { ++it; }
            return *this;
        }
    };

    template <typename S, typename Pred, typename Proj>
    struct remove_if_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(remove_if_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(remove_if_iterator, remove_if_sentinel, Pred, Proj);
    };

    template <typename V, typename Pred, typename Proj>
    requires concepts::removable_if_range<V, Pred, Proj>
    struct remove_if_view : std::ranges::view_interface<remove_if_view<V, Pred, Proj>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(remove_if_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(remove_if_iterator, remove_if_sentinel, Pred, Proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(pred, proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        Pred pred;
        Proj proj;

        GENEX_INLINE constexpr explicit remove_if_view(V rng, Pred pred, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<Pred> and
            std::is_nothrow_move_constructible_v<Proj>) :
            base_rng(std::move(rng)), pred(std::move(pred)), proj(std::move(proj)) {
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
    struct remove_if_fn {
        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::removable_if_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Pred pred, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::remove_if_view<V, Pred, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(proj)};
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::removable_if_range<Rng, Pred, Proj> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Pred pred, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::remove_if_view<V, Pred, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(proj)}; // .as_pointer_subrange();
        }

        template <typename Pred, typename Proj = meta::identity>
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const noexcept -> auto {
            return std::bind_back(
                remove_if_fn{}, std::move(pred), std::move(proj));
        }
    };

    inline constexpr remove_if_fn remove_if{};
}
