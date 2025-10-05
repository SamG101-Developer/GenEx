#pragma once
#include <functional>

#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj = meta::identity>
    concept filterable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::movable<I>;

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    concept filterable_range =
        input_range<Rng> and
        filterable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::filterable_iters<I, S, Pred, Proj>
    struct filter_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::forward_iterator_tag);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(filter_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(filter_iterator);

        Pred pred;
        Proj proj;

        GENEX_INLINE constexpr explicit filter_iterator(I it, S st, Pred pred, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<Pred> and
            std::is_nothrow_move_constructible_v<Proj>) :
            it(std::move(it)), st(std::move(st)), pred(std::move(pred)), proj(std::move(proj)) {
            while (this->it != this->st and not std::invoke(this->pred, std::invoke(this->proj, *this->it))) { ++this->it; }
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it) and std::is_nothrow_invocable_v<Pred&, std::invoke_result_t<Proj&, iter_reference_t<I>>>)
            -> filter_iterator& {
            do { ++it; } while (it != st and not std::invoke(pred, std::invoke(proj, *it)));
            return *this;
        }
    };

    template <typename S, typename Pred, typename Proj>
    struct filter_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(filter_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(filter_iterator, filter_sentinel, Pred, Proj);
    };

    template <typename V, typename Pred, typename Proj>
    requires concepts::filterable_range<V, Pred, Proj>
    struct filter_view : std::ranges::view_interface<filter_view<V, Pred, Proj>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(filter_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(filter_iterator, filter_sentinel, Pred, Proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(pred, proj);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        Pred pred;
        Proj proj;

        GENEX_INLINE constexpr explicit filter_view(V rng, Pred pred, Proj proj) noexcept(
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
    struct filter_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::filterable_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I it, S st, Pred pred, Proj proj = {}) const noexcept -> auto {
            return detail::filter_view{
                std::ranges::subrange{std::move(it), std::move(st)}, std::move(pred), std::move(proj)};
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::filterable_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const noexcept -> auto {
            return detail::filter_view{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(proj)};
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::filterable_range<Rng, Pred, Proj> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const noexcept -> auto {
            return detail::filter_view{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(proj)}; // .as_pointer_subrange();
        }

        template <typename Pred, typename Proj = meta::identity>
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const noexcept -> auto {
            return std::bind_back(
                filter_fn{}, std::move(pred), std::move(proj));
        }
    };

    inline constexpr filter_fn filter{};
}
