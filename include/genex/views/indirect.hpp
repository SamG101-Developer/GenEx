#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept indirectable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        requires(I it) { { **it }; };

    template <typename Rng>
    concept indirectable_range =
        input_range<Rng> and
        indirectable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::indirectable_iters<I, S>
    auto do_indirect(I first, S last) -> generator<decltype(**std::declval<I&>())> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield **first;
        }
    }
}


namespace genex::views {
    struct indirect_fn {
        template <typename I, typename S>
        requires detail::concepts::indirectable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_indirect(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::indirectable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_indirect(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(indirect_fn{});
        }
    };

    inline constexpr indirect_fn indirect{};
}
