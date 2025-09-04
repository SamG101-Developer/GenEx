#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
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


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::flattenable_iters<I, S>
    auto do_flatten(I first, S last) -> generator<range_value_t<iter_value_t<I>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            for (auto &&x : *first) { co_yield static_cast<range_value_t<iter_value_t<I>>>(x); }
        }
    }
}


namespace genex::views {
    struct flatten_fn {
        template <typename I, typename S>
            requires concepts::flattenable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_flatten(
                std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::flattenable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(flatten_fn{});
        }
    };

    EXPORT_GENEX_STRUCT(flatten);
}
