#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/size.hpp>


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

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        Pred pred;
        New new_val;
        Proj proj;

        GENEX_INLINE constexpr explicit replace_if_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            replace_if_iterator, it);

        GENEX_INLINE constexpr explicit replace_if_iterator(I it, S st, Pred pred, New new_val, Proj proj) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Pred, New, Proj>) :
            it(std::move(it)), st(std::move(st)), pred(std::move(pred)), new_val(std::move(new_val)), proj(std::move(proj)) {
        }

        GENEX_INLINE constexpr replace_if_iterator(replace_if_iterator &&other) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Pred, New, Proj>) :
            it(std::move(other.it)), st(std::move(other.st)), pred(std::move(other.pred)), new_val(std::move(other.new_val)), proj(std::move(other.proj)) {
        }

        GENEX_INLINE constexpr replace_if_iterator(const replace_if_iterator &other) noexcept(
            meta::all_of_v<std::is_nothrow_copy_constructible, I, S, Pred, New, Proj>) :
            it(other.it), st(other.st), pred(other.pred), new_val(other.new_val), proj(other.proj) {
        }

        GENEX_INLINE constexpr auto operator=(replace_if_iterator &&other) noexcept(
            meta::all_of_v<std::is_nothrow_move_assignable, I, S, Pred, New, Proj>) -> replace_if_iterator& {
            it = std::move(other.it);
            st = std::move(other.st);
            pred = std::move(other.pred);
            new_val = std::move(other.new_val);
            proj = std::move(other.proj);
            return *this;
        }

        GENEX_INLINE constexpr auto operator=(const replace_if_iterator &other) noexcept(
            meta::all_of_v<std::is_nothrow_copy_assignable, I, S, Pred, New, Proj>) -> replace_if_iterator& {
            it = other.it;
            st = other.st;
            pred = other.pred;
            new_val = other.new_val;
            proj = other.proj;
            return *this;
        }

        GENEX_INLINE constexpr auto operator*() noexcept(
            noexcept(*it) and
            std::is_nothrow_invocable_v<Proj, iter_reference_t<I>> and
            std::is_nothrow_invocable_v<Pred, std::invoke_result_t<Proj, iter_reference_t<I>>>) -> reference {
            if (std::invoke(pred, std::invoke(proj, *it))) { return new_val; }
            return *it;
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it) and
            std::is_nothrow_invocable_v<Proj, iter_reference_t<I>> and
            std::is_nothrow_invocable_v<Pred, std::invoke_result_t<Proj, iter_reference_t<I>>>) -> std::add_const_t<reference> {
            if (std::invoke(pred, std::invoke(proj, *it))) { return new_val; }
            return *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> replace_if_iterator& {
            ++it;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> replace_if_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct replace_if_sentinel { };

    template <typename I, typename S, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    replace_if_iterator(I, S, Pred, New, Proj) -> replace_if_iterator<I, S, Pred, New, Proj>;

    template <typename I, typename S, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(replace_if_iterator, replace_if_sentinel, I, S, Pred, New, Proj) {
        return it.it == it.st;
    }

    template <typename V, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_range<V, Pred, New, Proj>
    struct replace_if_view : std::ranges::view_interface<replace_if_view<V, Pred, New, Proj>> {
        V base_rng;
        Pred pred;
        New new_val;
        Proj proj;

        GENEX_INLINE constexpr explicit replace_if_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            replace_if_iterator, replace_if_sentinel, base_rng, pred, new_val, proj);

        GENEX_INLINE constexpr explicit replace_if_view(V rng, Pred pred, New new_val, Proj proj) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, V, Pred, New, Proj>) :
            base_rng(std::move(rng)), pred(std::move(pred)), new_val(std::move(new_val)), proj(std::move(proj)) {
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
    struct replace_if_fn {
        template <typename I, typename S, typename Pred, typename New, typename Proj = std::identity>
        requires detail::concepts::replaceable_if_iters<I, S, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(I it, S st, Pred pred, New new_val, Proj proj = {}) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, I, S, Pred, New, Proj>) {
            using V = std::ranges::subrange<I, S>;
            return detail::replace_if_view<V, Pred, New, Proj>{
                std::ranges::subrange{std::move(it), std::move(st)}, std::move(pred), std::move(new_val), std::move(proj)};
        }

        template <typename Rng, typename Pred, typename New, typename Proj = std::identity>
        requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Pred pred, New new_val, Proj proj = {}) const noexcept(
            meta::all_of_v<std::is_nothrow_constructible, Rng&&> and
            meta::all_of_v<std::is_nothrow_move_constructible, Pred, New, Proj>) {
            using V = std::views::all_t<Rng>;
            return detail::replace_if_view<V, Pred, New, Proj>{
                std::forward<Rng>(rng), std::move(pred), std::move(new_val), std::move(proj)};
        }

        template <typename Pred, typename New, typename Proj = std::identity>
        GENEX_INLINE constexpr auto operator()(Pred pred, New new_val, Proj proj = {}) const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, replace_if_fn> and
            meta::all_of_v<std::is_nothrow_move_constructible, Pred, New, Proj>) {
            return std::bind_back(
                replace_if_fn{}, std::move(pred), std::move(new_val), std::move(proj));
        }
    };

    inline constexpr replace_if_fn replace_if{};
}
