#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename F, typename Proj = meta::identity>
    concept transformable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_unary_invocable<Proj&, I> and
        std::indirectly_unary_invocable<F&, std::projected<I, Proj&>> and
        std::movable<I>;

    template <typename Rng, typename F, typename Proj = meta::identity>
    concept transformable_range =
        input_range<Rng> and
        transformable_iters<iterator_t<Rng>, sentinel_t<Rng>, F, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    struct transform_iterator {
        using reference = std::invoke_result_t<F&, std::invoke_result_t<Proj&, iter_reference_t<I>>>;
        using value_type = std::remove_cvref_t<reference>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(transform_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(transform_iterator);

        F f;
        Proj proj;

        GENEX_INLINE constexpr explicit transform_iterator(I it, S st, F f, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<F> and
            std::is_nothrow_move_constructible_v<Proj>) :
            it(std::move(it)), st(std::move(st)), f(std::move(f)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            std::is_nothrow_invocable_v<F&, std::invoke_result_t<Proj&, std::iter_reference_t<I>>>)
            -> reference {
            return std::invoke(f, std::invoke(proj, *it));
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(
            std::is_nothrow_copy_constructible_v<std::remove_cvref_t<reference>> ||
            std::is_nothrow_move_constructible_v<std::remove_cvref_t<reference>>)
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> transform_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, typename F, typename Proj>
    struct transform_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(transform_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(transform_iterator, transform_sentinel, F, Proj);
    };

    template <typename V, typename F, typename Proj>
    requires concepts::transformable_range<V, F, Proj>
    struct transform_view : std::ranges::view_interface<transform_view<V, F, Proj>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(transform_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(transform_iterator, transform_sentinel, F, Proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(f, proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        F f;
        Proj proj;

        GENEX_INLINE constexpr explicit transform_view(V rng, F f, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<F> and
            std::is_nothrow_move_constructible_v<Proj>) :
            base_rng(std::move(rng)), f(std::move(f)), proj(std::move(proj)) {
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
    struct transform_fn {
        template <typename I, typename S, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_iters<I, S, F, Proj>
        GENEX_INLINE constexpr auto operator()(I it, S st, F f, Proj proj = {}) const -> auto {
            return detail::transform_view{
                std::ranges::subrange{std::move(it), std::move(st)}, std::move(f), std::move(proj)};
        }

        template <typename Rng, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_range<Rng, F, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f, Proj proj = {}) const -> auto {
            return detail::transform_view{
                std::views::all(std::forward<Rng>(rng)), std::move(f), std::move(proj)};
        }

        template <typename Rng, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_range<Rng, F, Proj> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f, Proj proj = {}) const -> auto {
            return detail::transform_view{
                std::views::all(std::forward<Rng>(rng)), std::move(f), std::move(proj)}; // .as_pointer_subrange();
        }

        template <typename F, typename Proj = meta::identity>
        GENEX_INLINE constexpr auto operator()(F f, Proj proj = {}) const -> auto {
            return std::bind_back(transform_fn{}, std::move(f), std::move(proj));
        }
    };

    inline constexpr transform_fn transform{};
}
