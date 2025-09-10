#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename New>
    concept flattenable_with_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::convertible_to<New, range_value_t<iter_value_t<I>>>;

    template <typename Rng, typename New>
    concept flattenable_with_range =
        input_range<Rng> and
        flattenable_with_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename New>
        requires concepts::flattenable_with_iters<I, S, New>
    GENEX_NO_ASAN
    auto do_flatten_with(I first, S last, New &&sep) -> generator<range_value_t<iter_value_t<I>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            for (auto &&x : *first) { co_yield static_cast<range_value_t<iter_value_t<I>>>(x); }
            if (iterators::next(first) != last) { co_yield sep; }
        }
    }
}


namespace genex::views {
    struct flatten_with_fn {
        template <typename I, typename S, typename New>
            requires concepts::flattenable_with_iters<I, S, New>
        constexpr auto operator()(I first, S last, New &&sep) const -> auto {
            return detail::do_flatten_with(
                std::move(first), std::move(last), std::forward<New>(sep));
        }

        template <typename Rng, typename New>
            requires concepts::flattenable_with_range<Rng, New>
        constexpr auto operator()(Rng &&rng, New &&sep) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<New>(sep));
        }

        template <typename New>
        constexpr auto operator()(New &&sep) const -> auto {
            return std::bind_back(flatten_with_fn{}, std::forward<New>(sep));
        }
    };

    GENEX_EXPORT_STRUCT(flatten_with);
}
