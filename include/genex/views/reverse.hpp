#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/prev.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept reversible_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept reversible_range =
        bidirectional_range<Rng> and
        reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
    auto do_reverse(I first, S last) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (; first != last; --last) {
            co_yield static_cast<iter_value_t<I>>(*iterators::prev(last, 1, first));
        }
    }
}


namespace genex::views {
    struct reverse_fn {
        template <typename I, typename S>
        requires detail::concepts::reversible_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::coros::do_reverse(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::reversible_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_reverse(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(reverse_fn{});
        }
    };

    inline constexpr reverse_fn reverse{};
}
