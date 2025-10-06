#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept removable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*> and
        std::movable<I>;

    template <typename Rng, typename E, typename Proj>
    concept removable_range =
        input_range<Rng> and
        removable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename E, typename Proj>
    requires concepts::removable_iters<I, S, E, Proj>
    struct remove_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        E val;
        Proj proj;

        GENEX_INLINE constexpr explicit remove_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            remove_iterator, it);

        GENEX_INLINE constexpr explicit remove_iterator(I it, S st, E val, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<E> and
            std::is_nothrow_move_constructible_v<Proj> and
            noexcept(satisfy())) :
            it(std::move(it)), st(std::move(st)), val(std::move(val)), proj(std::move(proj)) {
            satisfy();
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it) and noexcept(satisfy())) -> remove_iterator& {
            ++it;
            satisfy();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
        noexcept(it++)) -> remove_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() noexcept(
            std::is_nothrow_invocable_v<Proj&, iter_reference_t<I>> and
            std::is_nothrow_invocable_v<operations::eq&, std::invoke_result_t<Proj&, iter_reference_t<I>>, E const*>) -> void {
            while (it != st and std::invoke(proj, *it) == val) { ++it; }
        }
    };

    struct remove_sentinel { };

    template <typename I, typename S, typename E, typename Proj>
    requires concepts::removable_iters<I, S, E, Proj>
    remove_iterator(I, S, E, Proj) -> remove_iterator<I, S, E, Proj>;

    template <typename I, typename S, typename E, typename Proj>
    requires concepts::removable_iters<I, S, E, Proj>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(remove_iterator, remove_sentinel, I, S, E, Proj) {
        return it.it == it.st;
    }

    template <typename V, typename E, typename Proj>
    struct remove_view : std::ranges::view_interface<remove_view<V, E, Proj>> {
        V base_rng;
        E val;
        Proj proj;

        GENEX_INLINE constexpr explicit remove_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            remove_iterator, remove_sentinel, base_rng, val, proj);

        GENEX_INLINE constexpr explicit remove_view(V rng, E val, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<E> and
            std::is_nothrow_move_constructible_v<Proj>) :
            base_rng(std::move(rng)), val(std::move(val)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    struct remove_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires detail::concepts::removable_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I it, S st, E val, Proj proj = {}) const noexcept -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::remove_view<V, E, Proj>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}, std::move(val), std::move(proj)};
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires detail::concepts::removable_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E val, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::remove_view<V, E, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(val), std::move(proj)};
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires detail::concepts::removable_range<Rng, E, Proj> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E val, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::remove_view<V, E, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(val), std::move(proj)}; // .as_pointer_subrange();
        }

        template <typename E, typename Proj = meta::identity>
        requires (not range<E>)
        GENEX_INLINE constexpr auto operator()(E val, Proj proj = {}) const noexcept -> auto {
            return std::bind_back(
                remove_fn{}, std::move(val), std::move(proj));
        }
    };

    inline constexpr remove_fn remove{};
}
