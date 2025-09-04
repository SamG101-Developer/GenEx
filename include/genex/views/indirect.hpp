#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
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


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::indirectable_iters<I, S>
    auto do_indirect(I first, S last) -> generator<decltype(**std::declval<I&>())> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield **first;
        }
    }
}


namespace genex::views {
    struct indirect_fn {
        template <typename I, typename S>
            requires concepts::indirectable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_indirect(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::indirectable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(indirect_fn{});
        }
    };

    EXPORT_GENEX_STRUCT(indirect);
}
