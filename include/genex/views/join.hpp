#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept flattenable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>>;

    template <typename Rng>
    concept flattenable_range =
        input_range<Rng> and
        flattenable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S>
    requires concepts::flattenable_iters<I, S>
    auto do_join(I first, S last) -> generator<range_value_t<iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            for (auto &&x : *first) {
                co_yield static_cast<range_value_t<iter_value_t<I>>>(x);
            }
        }
    }
}


namespace genex::views {
    struct flatten_fn {
        template <typename I, typename S>
        requires detail::concepts::flattenable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::coros::do_join(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::flattenable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_join(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return std::bind_back(flatten_fn{});
        }
    };

    inline constexpr flatten_fn flatten{};
}
