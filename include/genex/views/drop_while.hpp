#pragma once
#include <genex/concepts.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept droppable_while_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::movable<I>;

    template <typename Rng, typename Pred, typename Proj>
    concept droppable_while_range =
        input_range<Rng> and
        droppable_while_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::droppable_while_iters<I, S, Pred, Proj>
    struct drop_while_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_INLINE constexpr explicit drop_while_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            drop_while_iterator, it);

        GENEX_INLINE constexpr explicit drop_while_iterator(I it, S st, Pred, Proj) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> drop_while_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
        noexcept(it++)) -> drop_while_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct drop_while_sentinel { };

    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::droppable_while_iters<I, S, Pred, Proj>
    drop_while_iterator(I, S, Pred, Proj) -> drop_while_iterator<I, S, Pred, Proj>;

    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::droppable_while_iters<I, S, Pred, Proj>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(drop_while_iterator, drop_while_sentinel, I, S, Pred, Proj) {
        return it.it == it.st;
    }

    template <typename V, typename Pred, typename Proj>
    requires concepts::droppable_while_range<V, Pred, Proj>
    struct drop_while_view : std::ranges::view_interface<drop_while_view<V, Pred, Proj>> {
        V base_rng;
        Pred pred;
        Proj proj;

        GENEX_INLINE constexpr explicit drop_while_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            drop_while_iterator, drop_while_sentinel, base_rng, pred, proj);

        GENEX_INLINE constexpr explicit drop_while_view(V rng, Pred pred, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<Pred> and
            std::is_nothrow_move_constructible_v<Proj>) :
            base_rng(std::move(rng)), pred(std::move(pred)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng)) and
            std::is_nothrow_invocable_v<Pred&, std::invoke_result_t<Proj&, range_reference_t<V>>>) {
            auto it = iterators::begin(base_rng);
            const auto end_it = iterators::end(base_rng);
            while (it != end_it and std::invoke(pred, std::invoke(proj, *it))) { ++it; }
            return it;
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    struct drop_while_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::droppable_while_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I it, S st, Pred pred, Proj proj = {}) const noexcept -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::drop_while_view<V, Pred, Proj>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}, std::move(pred), std::move(proj)};
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::droppable_while_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Pred pred, Proj proj = {}) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::drop_while_view<V, Pred, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(proj)};
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::droppable_while_range<Rng, Pred, Proj> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Pred pred, Proj proj = {}) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::drop_while_view<V, Pred, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(pred), std::move(proj)}; // .as_pointer_subrange();
        }

        template <typename Pred, typename Proj = meta::identity>
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const noexcept -> auto {
            return std::bind_back(
                drop_while_fn{}, std::move(pred), std::move(proj));
        }
    };

    inline constexpr drop_while_fn drop_while{};
}
