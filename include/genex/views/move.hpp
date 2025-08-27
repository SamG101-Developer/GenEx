#pragma once
#include <coroutine>
#include <utility>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_move(I first, S last) -> generator<iter_value_t<I>&&> {
        for (; first != last; ++first) {
            co_yield std::move(*first);
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_move_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::movable<iter_value_t<I>>;

    template <typename Rng>
    concept can_move_range =
        input_range<Rng> and
        std::movable<range_value_t<Rng>>;

    DEFINE_VIEW(move) {
        template <typename I, typename S> requires can_move_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_move(std::move(first), std::move(last));
            return move_view(std::move(gen));
        }

        template <typename Rng> requires can_move_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            // Create a closure that takes a range and applies the move.
            return
                [FWD_CAPTURES()]<typename Rng> requires can_move_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(move);
}
