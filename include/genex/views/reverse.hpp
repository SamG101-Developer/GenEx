#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_reverse_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept can_reverse_range =
        bidirectional_range<Rng> and
        can_reverse_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_reverse_iters<I, S>
    auto do_reverse(I first, S last) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        for (; last != first; --last) {
            co_yield static_cast<iter_value_t<I>>(*iterators::prev(std::forward<decltype(last)>(last)));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(reverse) {
        template <typename I, typename S> requires concepts::can_reverse_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_reverse(std::move(first), std::move(last));
            return reverse_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_reverse_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            // Create a closure that takes a range and applies the reverse.
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_reverse_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(reverse);
}
