#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename F>
    concept for_eachable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::invocable<F, iter_value_t<I>>;

    template <typename Rng, typename F>
    concept for_eachable_range =
        input_range<Rng> and
        for_eachable_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S, typename F>
    requires concepts::for_eachable_iters<I, S, F>
    auto do_for_each(I first, S last, F f) -> void {
        for (; first != last; ++first) {
            std::invoke(f, *first);
        }
    }
}


namespace genex::views {
    struct for_each_fn {
        template <typename I, typename S, typename F>
        requires detail::concepts::for_eachable_iters<I, S, F>
        GENEX_INLINE constexpr auto operator()(I first, S last, F f) const {
            detail::coros::do_for_each(std::move(first), std::move(last), std::move(f));
        }

        template <typename Rng, typename F>
        requires detail::concepts::for_eachable_range<Rng, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f) const {
            auto [first, last] = iterators::iter_pair(rng);
            detail::coros::do_for_each(std::move(first), std::move(last), std::move(f));
        }

        template <typename F>
        requires (not range<F>)
        GENEX_INLINE constexpr auto operator()(F f) const {
            return std::bind_back(for_each_fn{}, std::move(f));
        }
    };

    inline constexpr for_each_fn for_each{};
}
