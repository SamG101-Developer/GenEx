#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename New>
    concept interspersable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::constructible_from<New, iter_value_t<I>> and
        std::common_with<iter_value_t<I>, New> and
        std::copy_constructible<New> and
        std::movable<I>;

    template <typename Rng, typename New>
    concept interspersable_range =
        input_range<Rng> and
        interspersable_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename New>
    requires concepts::interspersable_iters<I, S, New>
    struct intersperse_iterator {
        using reference = std::common_reference_t<iter_reference_t<I>, New const&>;
        using value_type = std::common_type_t<iter_value_t<I>, New>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;
        New sep;
        bool at_sep = false;

        GENEX_INLINE constexpr explicit intersperse_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            intersperse_iterator, it);

        GENEX_INLINE constexpr explicit intersperse_iterator(I it, S st, New sep) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<New>) :
            it(std::move(it)), st(std::move(st)), sep(std::move(sep)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(*it)) -> reference {
            return at_sep ? sep : *it;
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> intersperse_iterator& {
            if (at_sep) {
                at_sep = false;
                ++it;
            }
            else if (it != st) {
                at_sep = true;
            }
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int)
            -> intersperse_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct intersperse_sentinel { };

    template <typename I, typename S, typename New>
    requires concepts::interspersable_iters<I, S, New>
    intersperse_iterator(I, S, New) -> intersperse_iterator<I, S, New>;

    template <typename I, typename S, typename New>
    requires concepts::interspersable_iters<I, S, New>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(intersperse_iterator, intersperse_sentinel, I, S, New) {
        return iterators::next(it.it) == it.st and it.at_sep;
    }

    template <typename V, typename New>
    requires concepts::interspersable_range<V, New>
    struct intersperse_view : std::ranges::view_interface<intersperse_view<V, New>> {
        V base_rng;
        New sep;

        GENEX_INLINE constexpr explicit intersperse_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            intersperse_iterator, intersperse_sentinel, base_rng, sep);

        GENEX_INLINE constexpr explicit intersperse_view(V rng, New sep) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<New>) :
            base_rng(std::move(rng)), sep(std::move(sep)) {
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
            return operations::size(base_rng) * 2 - (operations::size(base_rng) > 0 ? 1 : 0);
        }
    };
}


namespace genex::views {
    struct intersperse_fn {
        template <typename I, typename S, typename New>
        requires detail::concepts::interspersable_iters<I, S, New>
        GENEX_INLINE constexpr auto operator()(I it, S st, New sep) const noexcept -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::intersperse_view<V, New>{
                std::ranges::subrange{std::move(it), std::move(st)}, std::move(sep)};
        }

        template <typename Rng, typename New>
        requires detail::concepts::interspersable_range<Rng, New>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New sep) const noexcept -> auto {
            using V = std::views::all_t<Rng>;
            return detail::intersperse_view<V, New>{
                std::forward<Rng>(rng), std::move(sep)};
        }

        template <typename New>
        GENEX_INLINE constexpr auto operator()(New sep) const noexcept -> auto {
            return std::bind_back(
                intersperse_fn{}, std::move(sep));
        }
    };

    inline constexpr intersperse_fn intersperse{};
}
