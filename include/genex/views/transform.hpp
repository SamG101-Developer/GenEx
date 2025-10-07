#pragma once
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
        using value_type = reference;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        F f;
        Proj proj;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            transform_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            transform_iterator, it);

        GENEX_INLINE constexpr explicit transform_iterator(I it, S st, F f, Proj proj) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, F, Proj>) :
            it(std::move(it)), st(std::move(st)), f(std::move(f)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            std::is_nothrow_invocable_v<F&, std::invoke_result_t<Proj&, std::iter_reference_t<I>>>)
            -> value_type {
            return std::invoke(f, std::invoke(proj, *it));
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> transform_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> transform_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct transform_sentinel { };

    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    transform_iterator(I, S, F, Proj) -> transform_iterator<I, S, F, Proj>;

    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(transform_iterator, transform_sentinel, I, S, F, Proj) {
        return it.it == it.st;
    }

    template <typename V, typename F, typename Proj>
    requires concepts::transformable_range<V, F, Proj>
    struct transform_view : std::ranges::view_interface<transform_view<V, F, Proj>> {
        V base_rng;
        F f;
        Proj proj;

        GENEX_INLINE constexpr explicit transform_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            transform_iterator, transform_sentinel, base_rng, f, proj);

        GENEX_INLINE constexpr explicit transform_view(V rng, F f, Proj proj) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, V, F, Proj>) :
            base_rng(std::move(rng)), f(std::move(f)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }

        GENEX_INLINE constexpr auto size() const noexcept(
            noexcept(operations::size(base_rng))) {
            return operations::size(base_rng);
        }
    };
}


namespace genex::views {
    struct transform_fn {
        template <typename I, typename S, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_iters<I, S, F, Proj>
        GENEX_INLINE constexpr auto operator()(I it, S st, F f, Proj proj = {}) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, F, Proj>) {
            using V = std::ranges::subrange<I, S>;
            return detail::transform_view<V, F, Proj>{
                std::ranges::subrange{std::move(it), std::move(st)}, std::move(f), std::move(proj)};
        }

        template <typename Rng, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_range<Rng, F, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f, Proj proj = {}) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&> and
            meta::all_of_v<std::is_nothrow_move_constructible, F, Proj>) {
            using V = std::views::all_t<Rng>;
            return detail::transform_view<V, F, Proj>{
                std::forward<Rng>(rng), std::move(f), std::move(proj)};
        }

        template <typename F, typename Proj = meta::identity>
        requires (not range<F>)
        GENEX_INLINE constexpr auto operator()(F f, Proj proj = {}) const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, transform_fn> and
            meta::all_of_v<std::is_nothrow_move_constructible, F, Proj>) {
            return std::bind_back(
                transform_fn{}, std::move(f), std::move(proj));
        }
    };

    inline constexpr transform_fn transform{};
}
