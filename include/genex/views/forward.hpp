#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_forward(I first, S last) -> generator<iter_value_t<I>&&> {
        for (; first != last; ++first) {
            co_yield std::forward<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_forward_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_forward_range =
        input_range<Rng>;

    DEFINE_VIEW(forward) {
        template <typename I, typename S> requires can_forward_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_forward(std::move(first), std::move(last));
            return forward_view(std::move(gen));
        }

        template <typename Rng> requires can_forward_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            // Create a closure that takes a range and applies the move.
            return
                [FWD_CAPTURES()]<typename Rng> requires can_forward_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(forward);
}
