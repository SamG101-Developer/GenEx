#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Pred, typename New, typename Proj>
    concept replaceable_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::convertible_to<New, iter_value_t<I>> and
        std::movable<I>;

    template <typename Rng, typename Pred, typename New, typename Proj>
    concept replaceable_if_range =
        input_range<Rng> and
        replaceable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, New, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    struct replace_if_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(replace_if_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(replace_if_iterator);

        Pred pred;
        New new_val;
        Proj proj;

        GENEX_INLINE constexpr explicit replace_if_iterator(I it, S st, Pred pred, New new_val, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<Pred> and
            std::is_nothrow_move_constructible_v<New> and
            std::is_nothrow_move_constructible_v<Proj>) :
            it(std::move(it)), st(std::move(st)), pred(std::move(pred)), new_val(std::move(new_val)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it) and
            std::is_nothrow_invocable_v<Proj, iter_reference_t<I>> and
            std::is_nothrow_invocable_v<Pred, std::invoke_result_t<Proj, iter_reference_t<I>>>)
            -> value_type {
            if (std::invoke(pred, std::invoke(proj, *it))) { return new_val; }
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(*it))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> replace_if_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, typename Pred, typename New, typename Proj>
    struct replace_if_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(replace_if_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(replace_if_iterator, replace_if_sentinel, Pred, New, Proj);
    };

    template <typename V, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_range<V, Pred, New, Proj>
    struct replace_if_view : std::ranges::view_interface<replace_if_view<V, Pred, New, Proj>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(replace_if_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(replace_if_iterator, replace_if_sentinel, Pred, New, Proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(pred, new_val, proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        Pred pred;
        New new_val;
        Proj proj;

        GENEX_INLINE constexpr explicit replace_if_view(V rng, Pred pred, New new_val, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<Pred> and
            std::is_nothrow_move_constructible_v<New> and
            std::is_nothrow_move_constructible_v<Proj>) :
            base_rng(std::move(rng)), pred(std::move(pred)), new_val(std::move(new_val)), proj(std::move(proj)) {
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
    struct replace_if_fn {
        template <typename Rng, typename Pred, typename New, typename Proj = std::identity>
        requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Pred pred, New new_val, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::replace_if_view<V, Pred, New, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(new_val), std::move(proj)};
        }

        template <typename Rng, typename Pred, typename New, typename Proj = std::identity>
        requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Pred pred, New new_val, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::replace_if_view<V, Pred, New, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(new_val), std::move(proj)}; // .as_pointer_subrange();
        }

        template <typename Pred, typename New, typename Proj = std::identity>
        GENEX_INLINE constexpr auto operator()(Pred pred, New new_val, Proj proj = {}) const noexcept -> auto {
            return std::bind_back(
                replace_if_fn{}, std::move(pred), std::move(new_val), std::move(proj));
        }
    };

    inline constexpr replace_if_fn replace_if{};
}
