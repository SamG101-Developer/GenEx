#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_copy_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::copy_constructible<iter_value_t<I>> and
        std::convertible_to<iter_reference_t<I>, iter_value_t<I>>;

    template <typename Rng>
    concept can_copy_range =
        input_range<Rng> and
        can_copy_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_copy_iters<I, S>
    auto do_copy(I first, S last) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(copy) {
        template <typename I, typename S> requires concepts::can_copy_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            // Call the copy inner function.
            auto gen = detail::do_copy(std::move(first), std::move(last));
            return copy_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_copy_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            // Call the copy inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            // Create a closure that takes a range and applies the copy.
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_copy_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(copy);
}
