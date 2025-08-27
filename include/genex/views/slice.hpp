#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_slice(I first, S last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) -> generator<iter_value_t<I>> {
        if (step == 0) { throw std::invalid_argument("Step cannot be zero."); }
        auto index = 0uz;
        for (; first != last; first += step, index += step) {
            if (index < start_index) { continue; }
            if (index >= end_index) { break; }
            co_yield *first;
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_slice_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_slice_range =
        input_range<Rng>;

    DEFINE_VIEW(slice) {
        template <typename I, typename S> requires can_slice_iters<I, S>
        constexpr auto operator()(I first, S last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_slice(std::move(first), std::move(last), start_index, end_index, step);
            return slice_view(std::move(gen));
        }

        template <typename Rng> requires can_slice_range<Rng>
        constexpr auto operator()(Rng &&rng, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), start_index, end_index, step);
        }

        constexpr auto operator()(const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> auto {
            // Create a closure that takes a range and applies the take.
            return
                [FWD_CAPTURES(start_index, end_index, step)]<typename Rng> requires can_slice_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), start_index, end_index, step);
            };
        }
    };

    EXPORT_GENEX_VIEW(slice);
}
