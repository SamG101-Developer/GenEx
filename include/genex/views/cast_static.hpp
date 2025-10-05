#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename To>
    concept static_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        requires(iter_value_t<I> &&from) { static_cast<To>(from); };

    template <typename Rng, typename To>
    concept static_castable_range =
        input_range<Rng> and
        static_castable_iters<iterator_t<Rng>, sentinel_t<Rng>, To>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename To>
    requires concepts::static_castable_iters<I, S, To>
    struct cast_static_iterator {
        using reference = std::add_lvalue_reference_t<To>;
        using value_type = To;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(cast_static_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(cast_static_iterator);

        constexpr explicit cast_static_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(static_cast<To>(*it)))
            -> value_type {
            return static_cast<To>(*it);
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> cast_static_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, typename To>
    struct cast_static_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(cast_static_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(cast_static_iterator, cast_static_sentinel, To);
    };

    template <typename V, typename To>
    requires (concepts::static_castable_range<V, To>) //  and viewable_range<Rng>)
    struct cast_static_view : std::ranges::view_interface<cast_static_view<V, To>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(cast_static_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(cast_static_iterator, cast_static_sentinel, To);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit cast_static_view(V rng) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)) {
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
    struct cast_static_fn {
        template <typename To, typename Rng>
        requires detail::concepts::static_castable_range<Rng, To>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::cast_static_view<V, To>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename To, typename Rng>
        requires detail::concepts::static_castable_range<Rng, To> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::cast_static_view<V, To>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        template <typename To>
        GENEX_INLINE constexpr auto operator()() const -> auto {
            return [*this]<typename Rng>(Rng &&rng) requires detail::concepts::static_castable_range<Rng, To> {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    inline constexpr cast_static_fn cast_static_{};
}


#define cast_static cast_static_.operator()
