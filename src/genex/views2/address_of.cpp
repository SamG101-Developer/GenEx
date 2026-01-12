module;
#include <genex/macros.hpp>

export module genex.views2.address_of;
export import genex.pipe;
import genex.concepts;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import genex.span;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept addressable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        requires(I it) { { std::addressof(*it) }; };

    template <typename Rng>
    concept addressable_range =
        input_range<Rng> and
        addressable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    /**
     * The @c address_of_iterator yields the addresses of the elements pointed to by the underlying iterator. Note that
     * there is no special "sentinel" type for the @c address_of_iterator, because the end is always known from the
     * underlying range.
     * @tparam I The type of the underlying iterator.
     * @tparam S The type of the underlying sentinel.
     */
    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    struct address_of_iterator {
        I it;

        using value_type = std::add_pointer_t<iter_value_t<I>>;
        using reference_type = std::add_pointer_t<iter_value_t<I>>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(address_of_iterator);

        GENEX_INLINE constexpr address_of_iterator() noexcept = default;

        GENEX_INLINE constexpr explicit address_of_iterator(I first) noexcept(
            std::is_nothrow_move_constructible_v<I>) :
            it(std::move(first)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return std::addressof(*self.it);
        }

        GENEX_VIEW_ITER_EQ(address_of_iterator, address_of_iterator) {
            return self.it == that.it;
        }
    };

    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    struct address_of_view {
        I it; S st;

        GENEX_INLINE constexpr address_of_view(I first, S last) noexcept(
            std::is_nothrow_move_constructible_v<I> and
            std::is_nothrow_move_constructible_v<S>) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_NODISCARD GENEX_INLINE constexpr auto begin(this Self&& self) noexcept(
            std::is_nothrow_constructible_v<address_of_view, I>) -> auto {
            return address_of_iterator<I, S>(self.it);
        }

        template <typename Self>
        GENEX_NODISCARD GENEX_INLINE constexpr auto end(this Self&& self) noexcept(
            std::is_nothrow_constructible_v<address_of_view, S>) -> auto {
            return address_of_iterator<I, S>(self.st);
        }

        template <typename Self>
        GENEX_NODISCARD GENEX_INLINE constexpr auto size(this Self &&self) noexcept(
            noexcept(iterators::distance(self.it, self.st))) -> auto {
            return iterators::distance(self.it, self.st);
        }
    };
}


namespace genex::views {
    struct address_of_fn {
        // template <typename I, typename S>
        // requires detail::concepts::addressable_iters<I, S> and std::contiguous_iterator<I>
        // GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
        //     std::is_nothrow_constructible_v<span<std::add_pointer_t<iter_value_t<I>>>, std::add_pointer_t<iter_value_t<I>>, std::size_t>) {
        //     using ptr_t = std::add_pointer_t<iter_value_t<I>>;
        //     auto ptr_begin = std::addressof(*first);
        //     return genex::span<ptr_t>(&ptr_begin, last - first);
        // }

        template <typename I, typename S>
        requires detail::concepts::addressable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
            std::is_nothrow_constructible_v<detail::impl::address_of_view<I, S>, I, S>) {
            return detail::impl::address_of_view(std::move(first), std::move(last));
        }

        // template <typename Rng>
        // requires detail::concepts::addressable_range<Rng> and std::contiguous_iterator<iterator_t<Rng>>
        // GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept {
        //     using ptr_t = std::add_pointer_t<iter_value_t<iterator_t<Rng>>>;
        //     auto [first, last] = iterators::iter_pair(rng);
        //     auto ptr_begin = std::addressof(*first);
        //     return genex::span<ptr_t>(&ptr_begin, last - first);
        // }

        template <typename Rng>
        requires detail::concepts::addressable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::address_of_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            std::is_nothrow_constructible_v<address_of_fn>) {
            return meta::bind_back(address_of_fn{});
        }
    };

    export inline constexpr address_of_fn address_of{};
}
