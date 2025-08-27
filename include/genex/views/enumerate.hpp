#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_enumerate(I first, S last) -> generator<std::pair<std::size_t, iter_value_t<I>>> {
        auto i = 0;
        for (; first != last; ++first) {
            co_yield {i++, *first};
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_enumerate_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_enumerate_range =
        input_range<Rng>;

    DEFINE_VIEW(enumerate) {
        template <typename I, typename S> requires can_enumerate_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            // Call the enumerate inner function.
            auto gen = detail::do_enumerate(std::move(first), std::move(last));
            return enumerate_view(std::move(gen));
        }

        template <typename Rng> requires can_enumerate_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            // Call the enumerate inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            // Create a closure that takes a range and applies the interleave.
            return
                [FWD_CAPTURES()]<typename Rng> requires can_enumerate_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(enumerate);
}
