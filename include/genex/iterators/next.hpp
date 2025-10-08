#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::detail::concepts {
    template <typename I, typename S = I>
    concept nextable_iters =
        std::input_or_output_iterator<I> and
        std::sentinel_for<S, I>;
}


namespace genex::iterators {
    struct next_fn {
        template <typename I>
        requires detail::concepts::nextable_iters<I> and std::copyable<I>
        GENEX_INLINE auto operator()(I it, const iter_difference_t<I> n = 1) const -> I {
            std::advance(it, n);
            return it;
        }

        template <typename I, typename S>
        requires detail::concepts::nextable_iters<I, S> and std::copyable<I>
        GENEX_INLINE auto operator()(I it, const iter_difference_t<I> n, const S end_it) const -> I {
            auto res = std::next(std::move(it), n); // todo: use bound
            return res;
        }
    };

    inline constexpr next_fn next{};
}
