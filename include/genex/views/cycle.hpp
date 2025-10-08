#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept cyclable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept cyclable_range =
        forward_range<Rng> and
        cyclable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S>
    requires concepts::cyclable_iters<I, S>
    auto do_cycle(I first, S last) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        while (true) {
            for (auto it = first; it != last; ++it) {
                co_yield static_cast<iter_value_t<I>>(*it);
            }
        }
    }
}


namespace genex::views {
    struct cycle_fn {
        template <typename I, typename S>
        requires detail::concepts::cyclable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::coros::do_cycle(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::cyclable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_cycle(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(cycle_fn{});
        }
    };

    inline constexpr cycle_fn cycle{};
}
