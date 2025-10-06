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

        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        using difference_type = difference_type_selector_t<I>;

        I it; S st;

        GENEX_INLINE constexpr explicit cast_smart_iterator() noexcept = default;

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            cast_smart_iterator, it);

        GENEX_INLINE constexpr explicit cast_smart_iterator(I it, S st) :
            it(std::move(it)), st(std::move(st)) {
        }

        GENEX_INLINE constexpr auto operator*() const // todo: optimize noexcept?
            -> value_type {
            if constexpr (unique_ptr<iter_value_t<I>>) {
                return std::unique_ptr<To>(dynamic_cast<To*>((*it).release()));
            } else if constexpr (shared_ptr<iter_value_t<I>>) {
                return std::dynamic_pointer_cast<To>(*it);
            } else if constexpr (weak_ptr<iter_value_t<I>>) {
                return std::dynamic_pointer_cast<To>(it->lock());
            }
            std::unreachable();
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(++it)) -> cast_smart_iterator& {
            ++it;
            return *this;
        }
    };

    struct cast_smart_sentinel { };

    template <typename I, typename S, typename To>
    requires concepts::smart_castable_iters<I, S, To>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(cast_smart_iterator, cast_smart_sentinel, I, S, To) {
        return it.it == it.st;
    }

    template <typename V, typename To>
    requires concepts::smart_castable_range<V, To>
    struct cast_smart_view : std::ranges::view_interface<cast_smart_view<V, To>> {
        V base_rng;

        GENEX_INLINE constexpr explicit cast_smart_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            cast_smart_iterator<iterator_t<V> COMMA sentinel_t<V> COMMA To>, cast_smart_sentinel, base_rng);

        GENEX_INLINE constexpr explicit cast_smart_view(V rng) noexcept(
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
    };
}


namespace genex::views {
    struct cast_smart_fn {
        template <typename To, typename I, typename S>
        requires detail::concepts::smart_castable_iters<I, S, To>
        GENEX_INLINE constexpr auto operator()(I it, S st) const -> auto {
            using V = std::ranges::subrange<I, S>;
            return detail::cast_smart_view<V, To>{
                std::ranges::subrange<I, S>{std::move(it), std::move(st)}};
        }

        template <typename To, typename Rng>
        requires detail::concepts::smart_castable_range<Rng, To> and contiguous_range<Rng> and borrowed_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> auto {
            using V = std::views::all_t<Rng>;
            return detail::cast_smart_view<V, To>{
                std::views::all(std::forward<Rng>(rng))}; // .as_pointer_subrange();
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
