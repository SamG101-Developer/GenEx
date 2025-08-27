#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_copy(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_copy_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::copy_constructible<iter_value_t<I>>;

    template <typename Rng>
    concept can_copy_range =
        input_range<Rng> and
        std::copy_constructible<range_value_t<Rng>>;

    DEFINE_VIEW(copy) {
        template <typename I, typename S> requires can_copy_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            // Call the copy inner function.
            auto gen = detail::do_copy(std::move(first), std::move(last));
            return copy_view(std::move(gen));
        }

        template <typename Rng> requires can_copy_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            // Call the copy inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            // Create a closure that takes a range and applies the copy.
            return
                [FWD_CAPTURES()]<typename Rng> requires can_copy_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(copy);
}
