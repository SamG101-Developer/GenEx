#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept borrowable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept borrowable_range =
        input_range<Rng> and
        borrowable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
    requires concepts::borrowable_iters<I, S>
    struct borrow_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(borrow_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(borrow_iterator);

        GENEX_INLINE constexpr explicit borrow_iterator(I it, S st) noexcept(
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
        -> borrow_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S>
    struct borrow_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(borrow_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(borrow_iterator, borrow_sentinel);
    };

    template <typename V>
    requires (concepts::borrowable_range<V>) //  and viewable_range<Rng>)
    struct borrow_view : std::ranges::view_interface<borrow_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(borrow_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(borrow_iterator, borrow_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        GENEX_INLINE constexpr explicit borrow_view(V rng) noexcept(
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
    struct borrow_fn {
        template <typename Rng>
        requires detail::concepts::borrowable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::borrow_view<V>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename Rng>
        requires detail::concepts::borrowable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::borrow_view<V>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        GENEX_INLINE constexpr auto operator()() const -> auto {
            return std::bind_back(
                borrow_fn{});
        }
    };

    inline constexpr borrow_fn borrow{};
}
