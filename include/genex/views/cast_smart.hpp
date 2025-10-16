#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename To, typename I, typename S>
    concept smart_ptr_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename To, typename Rng>
    concept smart_ptr_castable_range =
        input_range<Rng> and
        smart_ptr_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename To, typename I, typename S>
    requires (concepts::smart_ptr_castable_iters<To, I, S> and unique_ptr<iter_value_t<I>>)
    auto do_unique_ptr_cast_iter(I first, S last) -> generator<std::unique_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = dynamic_cast<To*>((*first).get())) {
                (*first).release();
                co_yield std::unique_ptr<To>(ptr);
            }
        }
    }

    template <typename To, typename I, typename S>
    requires (concepts::smart_ptr_castable_iters<To, I, S> and shared_ptr<iter_value_t<I>>)
    auto do_shared_ptr_cast_iter(I first, S last) -> generator<std::shared_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = std::dynamic_pointer_cast<To>(*first)) {
                co_yield ptr;
            }
        }
    }

    template <typename To, typename I, typename S>
    requires (concepts::smart_ptr_castable_iters<To, I, S> and weak_ptr<iter_value_t<I>>)
    auto do_weak_ptr_cast_iter(I first, S last) -> generator<std::weak_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = std::dynamic_pointer_cast<To>(first->lock())) {
                co_yield std::weak_ptr<To>(ptr);
            }
        }
    }
}


namespace genex::views {
    template <typename To>
    struct cast_smart_fn {
        template <typename I, typename S>
        requires detail::concepts::smart_ptr_castable_iters<To, I, S> and unique_ptr<iter_value_t<I>> and std::movable<iter_value_t<I>>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_unique_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename I, typename S>
        requires detail::concepts::smart_ptr_castable_iters<To, I, S> and shared_ptr<iter_value_t<I>>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_shared_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename I, typename S>
        requires detail::concepts::smart_ptr_castable_iters<To, I, S> and weak_ptr<iter_value_t<I>>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_weak_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::smart_ptr_castable_range<To, Rng> and unique_ptr<range_value_t<Rng>> and std::movable<range_value_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_unique_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::smart_ptr_castable_range<To, Rng> and shared_ptr<range_value_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_shared_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::smart_ptr_castable_range<To, Rng> and weak_ptr<range_value_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_weak_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(cast_smart_fn{});
        }
    };

    template <typename To>
    inline constexpr cast_smart_fn<To> cast_smart{};
}
