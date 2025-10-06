#pragma once
#include <utility>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename New>
    concept joinable_with_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::convertible_to<New, range_value_t<iter_value_t<I>>> and
        std::movable<I>;

    template <typename Rng, typename New>
    concept joinable_with_range =
        input_range<Rng> and
        joinable_with_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
    struct join_with_iterator {
        using reference = range_reference_t<iter_value_t<I>>;
        using value_type = range_value_t<iter_value_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        iterator_t<iter_value_t<I>> sub_it;
        iterator_t<iter_value_t<I>> sub_st;
        New sep;
        bool at_sep = false;

        GENEX_INLINE constexpr explicit join_with_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            join_with_iterator, it);

        GENEX_INLINE constexpr explicit join_with_iterator(I it, S st, New sep) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<New> and
            noexcept(satisfy())) :
            it(std::move(it)), st(std::move(st)), sep(std::move(sep)) {
            if (this->it != this->st) { satisfy(); }
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*sub_it)) -> reference {
            return at_sep ? sep : *sub_it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it) and noexcept(++sub_it) and noexcept(satisfy()))
            -> join_with_iterator& {
            if (at_sep) {
                at_sep = false;
                ++it;
                if (it != st) { satisfy(); }
            }
            else {
                ++sub_it;
                if (sub_it == sub_st and it != st) { at_sep = true; }
            }
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> join_with_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() noexcept(
            noexcept(iterators::begin(*it)) and
            noexcept(iterators::end(*it))) -> void {
            sub_it = iterators::begin(*it);
            sub_st = iterators::end(*it);
        }
    };

    struct join_with_sentinel { };

    template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
    join_with_iterator(I, S, New) -> join_with_iterator<I, S, New>;

    template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(join_with_iterator, join_with_sentinel, I, S, New) {
        return iterators::next(it.it) == it.st and (it.sub_it == it.sub_st or it.at_sep);
    }

    template <typename V, typename New>
    requires concepts::joinable_with_range<V, New>
    struct join_with_view : std::ranges::view_interface<join_with_view<V, New>> {
        V base_rng;
        New sep;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            join_with_iterator, join_with_sentinel, base_rng, sep);

        GENEX_INLINE constexpr explicit join_with_view(V it, New sep) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<New>) :
            base_rng(std::move(it)), sep(std::move(sep)) {
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
    struct join_with_fn {
        template <typename I, typename S, typename New>
        requires detail::concepts::joinable_with_iters<I, S, New>
        GENEX_INLINE constexpr auto operator()(I it, S st, New sep) const -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::join_with_view<V, New>{
                std::ranges::subrange{std::move(it), std::move(st)}, std::move(sep)};
        }

        template <typename Rng, typename New>
        requires detail::concepts::joinable_with_range<Rng, New>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New sep) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::join_with_view<V, New>{
                std::views::all(std::forward<Rng>(rng)), std::move(sep)};
        }

        template <typename Rng, typename New>
        requires detail::concepts::joinable_with_range<Rng, New> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New sep) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::join_with_view<V, New>{
                std::views::all(std::forward<Rng>(rng)), std::move(sep)}; // .as_pointer_subrange();
        }

        template <typename New>
        GENEX_INLINE constexpr auto operator()(New sep) const -> auto {
            return std::bind_back(
                join_with_fn{}, std::move(sep));
        }
    };

    inline constexpr join_with_fn join_with{};
}
