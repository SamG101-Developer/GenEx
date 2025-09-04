#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/prev.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept reversible_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename I, typename S>
    concept reversible_iters_optimized =
        std::random_access_iterator<I> and
        reversible_iters<I, S>;

    template <typename I, typename S>
    concept reversible_iters_unoptimized =
        std::input_iterator<I> and
        not std::random_access_iterator<I> and
        reversible_iters<I, S>;

    template <typename Rng>
    concept reversible_range =
        bidirectional_range<Rng> and
        reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::reversible_iters_optimized<I, S>
    auto do_reverse(I first, S last) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        while (first < last) {
            --last;
            co_yield *last;
        }
    }

    template <typename I, typename S>
        requires concepts::reversible_iters_unoptimized<I, S>
    auto do_reverse(I first, S last) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        while (first != last) {
            last = iterators::prev(last);
            co_yield *last;
        }
    }
}


namespace genex::views {
    struct reverse_fn {
        template <typename I, typename S>
            requires concepts::reversible_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_reverse(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::reversible_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(reverse_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(reverse);
}
