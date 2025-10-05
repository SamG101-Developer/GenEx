#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename To>
    concept smart_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        (not std::is_pointer_v<To> and not std::is_reference_v<To>) and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename Rng, typename To>
    concept smart_castable_range =
        input_range<Rng> and
        smart_castable_iters<iterator_t<Rng>, sentinel_t<Rng>, To>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename To>
    requires concepts::smart_castable_iters<I, S, To>
    struct cast_smart_iterator {
        using value_type = smart_pointer_cast_t<iter_value_t<I>, To>;
        using reference = std::add_lvalue_reference_t<To>;
        using pointer = value_type::element_type;

        GENEX_VIEW_ITERATOR_TYPE_DEFINITIONS(std::iterator_traits<I>::iterator_category);
        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(cast_smart_iterator);
        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(cast_smart_iterator);

        constexpr explicit cast_smart_iterator(I it, S st) :
            it(std::move(it)), st(std::move(st)) {
        }

        constexpr auto operator*() const noexcept
            -> value_type {
            if constexpr (unique_ptr<iter_value_t<I>>) {
                return std::unique_ptr<To>(dynamic_cast<To*>((*it).release()));
            } else if constexpr (shared_ptr<iter_value_t<I>>) {
                return std::dynamic_pointer_cast<To>(*it);
            } else if constexpr (weak_ptr<iter_value_t<I>>) {
                return std::dynamic_pointer_cast<To>(it->lock());
            }
        }

        constexpr auto operator->() const noexcept
            -> pointer {
            return operator*().get();
        }

        constexpr auto operator++() noexcept(noexcept(++it))
        -> cast_smart_iterator& {
            ++it;
            return *this;
        }
    };

    template <typename S, typename To>
    struct cast_smart_sentinel {
        GENEX_VIEW_SENTINEL_CTOR_DEFINITIONS(cast_smart_sentinel);
        GENEX_VIEW_SENTINEL_FUNC_DEFINITIONS(cast_smart_iterator, cast_smart_sentinel, To);
    };

    template <typename V, typename To>
    requires (concepts::smart_castable_range<V, To>) //  and viewable_range<Rng>)
    struct cast_smart_view : std::ranges::view_interface<cast_smart_view<V, To>> {
        GENEX_VIEW_VIEW_CTOR_DEFINITIONS(cast_smart_view);
        GENEX_VIEW_VIEW_TYPE_DEFINITIONS(cast_smart_iterator, cast_smart_sentinel, To);
        GENEX_VIEW_VIEW_FUNC_DEFINITIONS();
        GENEX_VIEW_VIEW_FUNC_DEFINITION_SUB_RANGE;

        constexpr explicit cast_smart_view(V rng) noexcept(std::is_nothrow_move_constructible_v<V>) :
            base_rng(std::move(rng)) {
        }

        GENEX_INLINE constexpr auto internal_begin() const {
            return iterators::begin(base_rng);
        }

        GENEX_INLINE constexpr auto internal_end() const {
            return iterators::end(base_rng);
        }
    };
}


namespace genex::views {
    struct cast_smart_fn {
        template <typename To, typename Rng>
        requires detail::concepts::smart_castable_range<Rng, To> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::cast_smart_view<V, To>{
                std::views::all(std::forward<Rng>(rng))}; // ; // .as_pointer_subrange();
        }

        template <typename To, typename Rng>
        requires detail::concepts::smart_castable_range<Rng, To>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::cast_smart_view<V, To>{
                std::views::all(std::forward<Rng>(rng))};
        }

        template <typename To>
        requires (not std::is_pointer_v<To> and not std::is_reference_v<To>)
        GENEX_INLINE constexpr auto operator()() const -> auto {
            return [*this]<typename Rng>(Rng &&rng) requires detail::concepts::smart_castable_range<Rng, To> {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    inline constexpr cast_smart_fn cast_smart_{};
}


#define cast_smart cast_smart_.operator()
