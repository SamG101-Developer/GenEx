#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept borrowable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept borrowable_range =
        input_range<Rng> and
        borrowable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S>
    requires concepts::borrowable_iters<I, S>
    auto do_borrow(I first, S last) -> generator<iter_value_t<I>&> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    struct borrow_fn {
        /**
         * Iterator interface for the borrow view.
         */
        template <typename I, typename S>
        requires detail::concepts::borrowable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::coros::do_borrow(std::move(first), std::move(last));
        }

        /**
         * Range interface for the borrow view.
         */
        template <typename Rng>
        requires detail::concepts::borrowable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_borrow(std::move(first), std::move(last));
        }

        /**
         * Pipe interface for the borrow view.
         */
        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(borrow_fn{});
        }
    };

    inline constexpr borrow_fn borrow{};
}
