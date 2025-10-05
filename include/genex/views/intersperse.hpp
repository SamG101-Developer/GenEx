#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
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

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(intersperse_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(intersperse_iterator);

        New sep;
        bool at_sep = false;

        GENEX_INLINE constexpr explicit intersperse_iterator(I it, S st, New sep) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S> and
            std::is_nothrow_move_constructible_v<New>) :
            it(std::move(it)), st(std::move(st)), sep(std::move(sep)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            if (at_sep) {
                return sep;
            }
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> intersperse_iterator& {
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

    template <typename S, typename New>
    struct intersperse_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(intersperse_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(intersperse_iterator, intersperse_sentinel, New);
    };

    template <typename V, typename New>
    requires concepts::interspersable_range<V, New>
    struct intersperse_view : std::ranges::view_interface<intersperse_view<V, New>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(intersperse_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(intersperse_iterator, intersperse_sentinel, New);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(sep);
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        New sep;

        GENEX_INLINE constexpr explicit intersperse_view(V rng, New sep) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<New>) :
            base_rng(std::move(rng)), sep(std::move(sep)) {
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
    struct intersperse_fn {
        template <typename Rng, typename New>
        requires detail::concepts::interspersable_range<Rng, New>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New sep) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::intersperse_view<V, New>{
                std::views::all(std::forward<Rng>(rng)), std::move(sep)};
        }

        template <typename Rng, typename New>
        requires detail::concepts::interspersable_range<Rng, New> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New sep) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::intersperse_view<V, New>{
                std::views::all(std::forward<Rng>(rng)), std::move(sep)}; // .as_pointer_subrange();
        }

        template <typename New>
        GENEX_INLINE constexpr auto operator()(New sep) const noexcept -> auto {
            return std::bind_back(
                intersperse_fn{}, std::move(sep));
        }
    };

    inline constexpr intersperse_fn intersperse{};
}
