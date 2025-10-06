#pragma once
#include <functional>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Old, typename New, typename Proj>
    concept replaceable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<Old> const*> and
        std::convertible_to<New, iter_value_t<I>> and
        std::movable<I>;

    template <typename Rng, typename Old, typename New, typename Proj>
    concept replaceable_range =
        input_range<Rng> and
        replaceable_iters<iterator_t<Rng>, sentinel_t<Rng>, Old, New, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Old, typename New, typename Proj>
    requires concepts::replaceable_iters<I, S, Old, New, Proj>
    struct replace_iterator {
        using reference = iter_value_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        Old old_val;
        New new_val;
        Proj proj;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            replace_iterator, it);

        GENEX_INLINE constexpr explicit replace_iterator(I it, S st, Old old_val, New new_val, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<Old> and
            std::is_nothrow_move_constructible_v<New> and
            std::is_nothrow_move_constructible_v<Proj>) :
            it(std::move(it)), st(std::move(st)), old_val(std::move(old_val)), new_val(std::move(new_val)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it) and
            std::is_nothrow_invocable_v<Proj, iter_reference_t<I>> and
            std::is_nothrow_invocable_v<std::equal_to<>, std::invoke_result_t<Proj, iter_reference_t<I>>, Old>) -> reference {
            if (std::invoke(proj, *it) == old_val) { return new_val; }
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> replace_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
        noexcept(it++)) -> replace_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct replace_sentinel { };

    template <typename I, typename S, typename Old, typename New, typename Proj>
    requires concepts::replaceable_iters<I, S, Old, New, Proj>
    replace_iterator(I, S, Old, New, Proj) -> replace_iterator<I, S, Old, New, Proj>;

    template <typename I, typename S, typename Old, typename New, typename Proj>
    requires concepts::replaceable_iters<I, S, Old, New, Proj>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(replace_iterator, replace_sentinel, I, S, Old, New, Proj) {
        return it.it == it.st;
    }

    template <typename V, typename Old, typename New, typename Proj>
    requires (concepts::replaceable_range<V, Old, New, Proj>)
    struct replace_view : std::ranges::view_interface<replace_view<V, Old, New, Proj>> {
        V base_rng;
        Old old_val;
        New new_val;
        Proj proj;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            replace_iterator, replace_sentinel, base_rng, old_val, new_val, proj);

        GENEX_INLINE constexpr explicit replace_view(V rng, Old old_val, New new_val, Proj proj) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<Old> and
            std::is_nothrow_move_constructible_v<New> and
            std::is_nothrow_move_constructible_v<Proj>) :
            base_rng(std::move(rng)), old_val(std::move(old_val)), new_val(std::move(new_val)), proj(std::move(proj)) {
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
    struct replace_fn {
        template <typename I, typename S, typename Old, typename New, typename Proj = std::identity>
        requires detail::concepts::replaceable_iters<I, S, Old, New, Proj>
        GENEX_INLINE constexpr auto operator()(I it, S st, Old old_val, New new_val, Proj proj = {}) const -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::replace_view<V, Old, New, Proj>{
                std::ranges::subrange{std::move(it), std::move(st)}, std::move(old_val), std::move(new_val), std::move(proj)};
        }

        template <typename Rng, typename Old, typename New, typename Proj = std::identity>
        requires detail::concepts::replaceable_range<Rng, Old, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Old old_val, New new_val, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::replace_view<V, Old, New, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(old_val), std::move(new_val), std::move(proj)};
        }

        template <typename Rng, typename Old, typename New, typename Proj = std::identity>
        requires detail::concepts::replaceable_range<Rng, Old, New, Proj> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Old old_val, New new_val, Proj proj = {}) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::replace_view<V, Old, New, Proj>{
                std::views::all(std::forward<Rng>(rng)), std::move(old_val), std::move(new_val), std::move(proj)}; // .as_pointer_subrange();
        }

        template <typename Old, typename New, typename Proj = std::identity>
        GENEX_INLINE constexpr auto operator()(Old old_val, New new_val, Proj proj = {}) const noexcept -> auto {
            return std::bind_back(
                replace_fn{}, std::move(old_val), std::move(new_val), std::move(proj));
        }
    };

    inline constexpr replace_fn replace{};
}
