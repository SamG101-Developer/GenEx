#pragma once
#include <utility>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept joinable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::movable<I>;

    template <typename Rng>
    concept joinable_range =
        input_range<Rng> and
        joinable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
    struct join_iterator {
        using reference = range_reference_t<iter_value_t<I>>;
        using value_type = range_value_t<iter_value_t<I>>;
        using pointer = std::add_pointer_t<value_type>;

        using _outer_cat = typename std::iterator_traits<I>::iterator_category;
        using _inner_cat = typename std::iterator_traits<iterator_t<iter_value_t<I>>>::iterator_category;

        using iterator_category = std::conditional_t<
        std::is_same_v<_outer_cat, std::random_access_iterator_tag> &&
        std::is_same_v<_inner_cat, std::random_access_iterator_tag>,
        std::random_access_iterator_tag,
        std::conditional_t<
            std::is_same_v<_outer_cat, std::bidirectional_iterator_tag> &&
            std::is_same_v<_inner_cat, std::bidirectional_iterator_tag>,
            std::bidirectional_iterator_tag,
            std::conditional_t<
                std::is_same_v<_outer_cat, std::forward_iterator_tag> &&
                std::is_same_v<_inner_cat, std::forward_iterator_tag>,
                std::forward_iterator_tag,
                std::input_iterator_tag
                >
            >
        >;

        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        iter_value_t<I> sub_rng;
        iterator_t<iter_value_t<I>> sub_it;
        iterator_t<iter_value_t<I>> sub_st;

        GENEX_INLINE constexpr explicit join_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            join_iterator, it);

        GENEX_INLINE constexpr explicit join_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            noexcept(satisfy())) :
            it(std::move(it)), st(std::move(st)) {
            if (this->it != this->st) { satisfy(); }
        }

        GENEX_INLINE constexpr join_iterator(join_iterator &&other) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<iter_value_t<I>> and
            std::is_nothrow_move_constructible_v<iterator_t<iter_value_t<I>>>) :
            it(std::move(other.it)),
            st(std::move(other.st)),
            sub_rng(std::move(other.sub_rng)),
            sub_it(std::move(other.sub_it)),
            sub_st(std::move(other.sub_st)) {
            if (this->it != this->st) { satisfy(); }
        }

        GENEX_INLINE constexpr join_iterator &operator=(join_iterator &&other) noexcept(
            std::is_nothrow_move_assignable_v<I> and
            std::is_nothrow_move_assignable_v<S> and
            std::is_nothrow_move_assignable_v<iter_value_t<I>> and
            std::is_nothrow_move_assignable_v<iterator_t<iter_value_t<I>>>) {
            it = std::move(other.it);
            st = std::move(other.st);
            sub_rng = std::move(other.sub_rng);
            sub_it = std::move(other.sub_it);
            sub_st = std::move(other.sub_st);
            if (this->it != this->st) { satisfy(); }
            return *this;
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*sub_it)) -> reference {
            return *sub_it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it) and
            noexcept(++sub_it) and
            noexcept(satisfy())) -> join_iterator& {

            if (it == st) return *this;

            ++sub_it;
            if (sub_it == sub_st) {
                ++it;
                if (it != st) { satisfy(); }
            }
            return *this;
        }


        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(it++)) -> join_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        GENEX_INLINE auto satisfy() noexcept(
            noexcept(iterators::begin(*it)) and
            noexcept(iterators::end(*it))) -> void {

            for (; it != st; ++it) {
                sub_rng = iter_value_t<I>(*it);
                sub_it = iterators::begin(sub_rng);
                sub_st = iterators::end(sub_rng);

                if (sub_it != sub_st)
                    return;
            }

            sub_it = {};
            sub_st = {};
        }
    };

    struct join_sentinel { };

    template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
    join_iterator(I, S) -> join_iterator<I, S>;

    template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(join_iterator, join_sentinel, I, S) {
        return it.it == it.st;
    }

    template <typename V>
    requires concepts::joinable_range<V>
    struct join_view : std::ranges::view_interface<join_view<V>> {
        V base_rng;

        GENEX_INLINE constexpr explicit join_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            join_iterator, join_sentinel, base_rng);

        GENEX_INLINE constexpr explicit join_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)) {
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
            noexcept(operations::size(base_rng))) -> range_size_t<V> {
            auto total_size = static_cast<range_size_t<V>>(0);
            for (auto const& sub_rng : base_rng) { total_size += operations::size(sub_rng); }
            return total_size;
        }
    };
}


namespace genex::views {
    struct join_fn {
        template <typename I, typename S>
        requires detail::concepts::joinable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I it, S st) const noexcept -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::join_view<V>{
                std::ranges::subrange{std::move(it), std::move(st)}};
        }

        template <typename Rng>
        requires detail::concepts::joinable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng) const noexcept -> auto {
            return detail::join_view{
                std::forward<Rng>(rng)};
        }

        GENEX_INLINE constexpr auto operator()() const noexcept -> auto {
            return std::bind_back(
                join_fn{});
        }
    };

    constexpr inline join_fn join{};
}
