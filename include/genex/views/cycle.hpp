#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept cyclable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept cyclable_range =
        forward_range<Rng> and
        cyclable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::cyclable_iters<I, S>
    GENEX_NO_ASAN
    auto do_cycle(I first, S last) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
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
            requires concepts::cyclable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_cycle(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::cyclable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(cycle_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(cycle);
}
