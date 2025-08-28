#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_forward_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        std::movable<std::remove_reference_t<iter_reference_t<I>>>;

    template <typename Rng>
    concept can_forward_range =
        input_range<Rng> and
        can_forward_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_forward_iters<I, S>
    auto do_forward(I first, S last) -> generator<std::remove_reference_t<iter_reference_t<I>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::move(*first);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(forward) {
        template <typename I, typename S> requires concepts::can_forward_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_forward(std::move(first), std::move(last));
            return forward_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_forward_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            // Create a closure that takes a range and applies the move.
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_forward_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(forward);
}
