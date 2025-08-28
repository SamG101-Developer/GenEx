#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_enumerate_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept can_enumerate_range =
        input_range<Rng> and
        can_enumerate_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_enumerate_iters<I, S>
    auto do_enumerate(I first, S last) -> generator<std::pair<std::size_t, iter_value_t<I>>> {
        if (first == last) { co_return; }
        auto i = 0;
        for (; first != last; ++first) {
            co_yield {i++, static_cast<iter_value_t<I>>(*first)};
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(enumerate) {
        template <typename I, typename S> requires concepts::can_enumerate_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            // Call the enumerate inner function.
            auto gen = detail::do_enumerate(std::move(first), std::move(last));
            return enumerate_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_enumerate_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            // Call the enumerate inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        constexpr auto operator()() const -> auto {
            // Create a closure that takes a range and applies the enumerate.
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_enumerate_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(enumerate);
}
