#pragma once
#include <genex/views2/view_base.hpp>


namespace genex::views2::detail::concepts {
    template <typename I, typename S>
    concept addressable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        requires(I it) { { std::addressof(*it) }; };

    template <typename Rng>
    concept addressable_range =
        input_range<Rng> and
        addressable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views2::detail::impl {
    struct address_sentinel {};

    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    struct address_of_iterator : view_iterator_base {
        I it; S st;

        using value_type = std::add_pointer_t<iter_value_t<I>>;
        using reference_type = std::add_pointer_t<iter_value_t<I>>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_FORWARD_ITERATOR_OPERATIONS;

        GENEX_INLINE constexpr address_of_iterator() = default;

        GENEX_INLINE constexpr address_of_iterator(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return std::addressof(*self.it);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(address_sentinel) {
            return self.it == self.st;
        }
    };

    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    struct address_of_view : view_base {
        I it; S st;

        GENEX_INLINE constexpr address_of_view(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return address_of_iterator{self.it, self.st};
        }

        template <typename Self>
        GENEX_ITER_END {
            return address_sentinel{};
        }
    };
}


namespace genex::views2 {
    struct address_of_fn {
        template <typename I, typename S>
        requires detail::concepts::addressable_iters<I, S> and std::contiguous_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            using ptr_t = std::add_pointer_t<iter_value_t<I>>;
            auto ptr_begin = std::addressof(*first);
            return genex::span<ptr_t>(&ptr_begin, last - first);
        }

        template <typename I, typename S>
        requires detail::concepts::addressable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::address_of_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::addressable_range<Rng> and std::contiguous_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            using ptr_t = std::add_pointer_t<iter_value_t<iterator_t<Rng>>>;
            auto [first, last] = iterators::iter_pair(rng);
            auto ptr_begin = std::addressof(*first);
            return genex::span<ptr_t>(&ptr_begin, last - first);
        }

        template <typename Rng>
        requires detail::concepts::addressable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::address_of_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(address_of_fn{});
        }
    };

    inline constexpr address_of_fn address_of{};
}
