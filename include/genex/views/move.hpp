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
    concept movable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>>;

    template <typename Rng>
    concept movable_range =
        input_range<Rng> and
        movable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::movable_iters<I, S>
    auto do_move(I first, S last) -> generator<iter_value_t<I>&&> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::move(*first);
        }
    }
}


namespace genex::views {
    struct move_fn {
        template <typename I, typename S>
            requires concepts::movable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_move(std::move(first), std::move(last));
        }

        template <typename Rng> requires concepts::movable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(move_fn{});
        }
    };

    EXPORT_GENEX_STRUCT(move);
}
