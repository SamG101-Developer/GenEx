#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_borrow(I first, S last) -> generator<iter_reference_t<I>> {
        for (; first != last; ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_borrow_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_borrow_range =
        input_range<Rng>;

    DEFINE_VIEW(borrow) {
        template <typename I, typename S> requires can_borrow_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            // Call the borrow inner function.
            auto gen = detail::do_borrow(first, last);
            return borrow_view(std::move(gen));
        }

        template <typename Rng> requires can_borrow_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            // Call the borrow inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            // Create a closure that takes a range and applies the borrow.
            return
                [FWD_CAPTURES()]<typename Rng> requires can_borrow_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(borrow);
}
