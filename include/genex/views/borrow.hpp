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
    concept can_borrow_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept can_borrow_range =
        input_range<Rng> and
        can_borrow_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_borrow_iters<I, S>
    auto do_borrow(I first, S last) -> generator<iter_value_t<I>&> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(borrow) {
        template <typename I, typename S> requires concepts::can_borrow_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            // Call the borrow inner function.
            auto gen = detail::do_borrow(std::move(first), std::move(last));
            return borrow_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_borrow_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            // Call the borrow inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            // Create a closure that takes a range and applies the borrow.
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_borrow_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(borrow);
}
