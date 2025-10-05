#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept takeable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::movable<I>;

    template <typename Rng, typename Int>
    concept takeable_range =
        input_range<Rng> and
        takeable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Int>
    requires concepts::takeable_iters<I, S, Int>
    struct take_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(take_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(take_iterator);

        GENEX_INLINE constexpr explicit take_iterator(I it, S st) noexcept(
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
            -> take_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, typename Int>
    struct take_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(take_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(take_iterator, take_sentinel, Int);
    };

    template <typename V, typename Int>
    requires concepts::takeable_range<V, Int>
    struct take_view : std::ranges::view_interface<take_view<V, Int>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(take_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(take_iterator, take_sentinel, Int);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        Int take_n;

        GENEX_INLINE constexpr explicit take_view(V rng, Int n) noexcept(
            std::is_nothrow_move_constructible_v<V> and
            std::is_nothrow_move_constructible_v<Int>) :
            base_rng(std::move(rng)), take_n(std::max(0, n)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(noexcept(iterators::begin(base_rng))) {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(noexcept(iterators::begin(base_rng))) requires random_access_range<V> {
            return iterators::next(iterators::begin(base_rng), take_n, iterators::end(base_rng));
        }
    };
}


namespace genex::views {
    struct take_fn {
        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int n) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::take_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), std::move(n)};
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int n) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::take_view<V, Int>{
                std::views::all(std::forward<Rng>(rng)), std::move(n)}; // .as_pointer_subrange();
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int n) const noexcept -> auto {
            return std::bind_back(
                take_fn{}, std::move(n));
        }
    };

    inline constexpr take_fn take{};
}
