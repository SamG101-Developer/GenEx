#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept borrowable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept borrowable_range =
        input_range<Rng> and
        borrowable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I>
    requires std::input_iterator<I> and std::indirectly_readable<I>
    struct borrow_iterator {
        using reference = iter_reference_t<I>;
        using value_type = iter_value_t<I>;
        using pointer = std::add_pointer_t<value_type>;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(GENEX_ITER_CONCEPT_FOLLOW_THROUGH);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(borrow_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(borrow_iterator);

        constexpr auto operator*() const noexcept
            -> reference {
            return *it;
        }

        constexpr auto operator->() const noexcept
            -> pointer {
            if constexpr (std::is_pointer_v<I>) {
                return it;
            } else {
                return std::addressof(*it);
            }
        }
    };

    template <typename S>
    struct borrow_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(borrow_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(borrow_iterator, borrow_sentinel);
    };

    template <typename I, typename S>
    requires concepts::borrowable_iters<I, S>
    struct borrow_from_iters {
        GENEX_VIEW_ITERSENT_CTOR_DEFINITIONS(borrow_from_iters);
        GENEX_VIEW_ITERSENT_TYPE_DEFINITIONS(borrow_iterator, borrow_sentinel);
        GENEX_VIEW_ITERSENT_FUNC_DEFINITIONS;
    };

    template <typename V>
    requires (concepts::borrowable_range<V>) //  and viewable_range<Rng>)
    struct borrow_view : std::ranges::view_interface<borrow_view<V>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(borrow_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(borrow_iterator, borrow_sentinel);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS;
    };
}


namespace genex::views {
    struct borrow_fn {
        template <typename I, typename S>
        requires detail::concepts::borrowable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::borrow_from_iters<I, S>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::borrowable_range<Rng> and contiguous_range<Rng> and borrowed_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::borrow_view<V>{std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
        }

        template <typename Rng>
        requires detail::concepts::borrowable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::borrow_view<V>{std::views::all(std::forward<Rng>(rng))};
        }

        constexpr auto operator()() const -> auto {
            return std::bind_front(borrow_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(borrow);
}
