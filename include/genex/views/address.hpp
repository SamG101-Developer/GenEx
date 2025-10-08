#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
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


namespace genex::views::detail::coros {
    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    auto do_address(I first, S last) -> generator<std::add_pointer_t<iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield std::addressof(*first);
        }
    }
}


namespace genex::views {
    struct address_fn {
        /**
         * Iterator interface for the address view.
         */
        template <typename I, typename S>
        requires detail::concepts::addressable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::coros::do_address(std::move(first), std::move(last));
        }

        /**
         * Range interface for the address view.
         */
        template <typename Rng>
        requires detail::concepts::addressable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_address(std::move(first), std::move(last));
        }

        /**
         * Pipe interface for the address view.
         */
        GENEX_INLINE auto operator()() const {
            return meta::bind_back(address_fn{});
        }
    };

    inline constexpr address_fn address{};
}
