#pragma once
#include <functional>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/next.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept takeable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::movable<I>;

    template <typename Rng, typename Int>
    concept takeable_last_range =
        input_range<Rng> and
        takeable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
    requires concepts::takeable_last_iters<I, S, Int>
    struct take_last_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(take_last_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(take_last_iterator);

        GENEX_INLINE constexpr explicit take_last_iterator(I it, S st) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(noexcept(*it))
            -> reference {
            return *it;
        }

        GENEX_INLINE constexpr auto operator->() const noexcept(noexcept(std::addressof(*it)))
            -> pointer {
            GENEX_ITERATOR_PROXY_ACCESS
        }

        GENEX_INLINE constexpr auto operator++() noexcept(noexcept(++it))
            -> take_last_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, typename Int>
    struct take_last_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(take_last_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(take_last_iterator, take_last_sentinel, Int);
    };

    template <typename V, typename Int>
    requires concepts::takeable_last_range<V, Int>
    struct take_last_view : std::ranges::view_interface<take_last_view<V, Int>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(take_last_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(take_last_iterator, take_last_sentinel, Int);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        Int take_n;

        GENEX_INLINE constexpr explicit take_last_view(V rng, Int take_n) noexcept(
            std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)), take_n(std::move(take_n)) {
        }

        GENEX_INLINE constexpr auto internal_begin() noexcept(noexcept(iterators::begin(base_rng))) {
            const auto dist = iterators::distance(base_rng);
            const auto cut = dist > take_n ? dist - take_n : 0;
            return iterators::next(iterators::begin(base_rng), cut, iterators::end(base_rng));
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::end(base_rng))) {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    struct take_last_fn {
        template <typename Rng, typename Int>
        requires detail::concepts::takeable_last_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int n) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::take_last_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), std::move(n)};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_last_range<Rng, Int> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, Int n) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::take_last_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), std::move(n)}; // .as_pointer_subrange();
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int n) const noexcept -> auto {
            return std::bind_back(
                take_last_fn{}, std::move(n));
        }
    };

    inline constexpr take_last_fn take_last{};
}
