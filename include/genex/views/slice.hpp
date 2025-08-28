#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_slice_iters_optimized =
        std::random_access_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename I, typename S>
    concept can_slice_iters_unoptimized =
        not can_slice_iters_optimized<I, S> and
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename I, typename S>
    concept can_slice_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept can_slice_range =
        input_range<Rng> and
        can_slice_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_slice_iters_unoptimized<I, S>
    auto do_slice(I first, S last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) -> generator<iter_value_t<I>> {
        if (step <= 0) { throw std::invalid_argument("Step cannot be <= 0."); }
        if (first == last) { co_return; }
        auto index = 0uz;
        for (; first != last; first += step, ++index) {
            if (index < start_index) { continue; }
            if (index >= end_index) { break; }
            if ((index - start_index) % step == 0) { co_yield static_cast<iter_value_t<I>>(*first); }
        }
    }

    template <typename I, typename S> requires concepts::can_slice_iters_optimized<I, S>
    auto do_slice(I first, S last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) -> generator<iter_value_t<I>> {
        if (step <= 0) { throw std::invalid_argument("Step cannot be <= 0."); }
        if (first == last) { co_return; }
        const auto distance = static_cast<std::size_t>(last - first);
        const auto real_start = std::min(start_index, distance);
        const auto real_end = std::min(end_index, distance);
        for (auto index = real_start; index < real_end; index += step) {
            co_yield static_cast<iter_value_t<I>>(*(first + index));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(slice) {
        template <typename I, typename S> requires concepts::can_slice_iters<I, S>
        auto operator()(I first, S last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> auto {
            // Call the slice inner function.
            auto gen = detail::do_slice(std::move(first), std::move(last), start_index, end_index, step);
            return slice_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_slice_range<Rng>
        auto operator()(Rng &&rng, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> auto {
            // Call the slice inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), start_index, end_index, step);
        }

        auto operator()(const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> auto {
            // Create a closure that takes a range and applies the slice.
            return
                [FWD_CAPTURES(start_index, end_index, step)]<typename Rng> requires concepts::can_slice_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), start_index, end_index, step);
            };
        }
    };

    EXPORT_GENEX_VIEW(slice);
}
