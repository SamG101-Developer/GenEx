#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_reverse(I first, S last) -> generator<iter_value_t<I>> {
        for (; last != first; --last) {
            co_yield *iterators::prev(std::forward<decltype(last)>(last));
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_reverse_iters =
        bidirectional_iterator<I> and
        sentinel_for<S, I> and
        std::same_as<I, S>;

    template <typename Rng>
    concept can_reverse_range =
        bidirectional_range<Rng> and
        common_range<Rng>;

    DEFINE_VIEW(reverse) {
        template <typename I, typename S> requires can_reverse_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_reverse(first, last);
            return reverse_view(std::move(gen));
        }

        template <typename Rng> requires can_reverse_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            // Create a closure that takes a range and applies the reverse.
            return
                [FWD_CAPTURES()]<typename Rng> requires can_reverse_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(reverse);
}
