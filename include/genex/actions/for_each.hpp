#pragma once
#include <functional>
#include <utility>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::details::concepts {
    template <typename I, typename S, typename F>
    concept for_eachable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::invocable<F, iter_value_t<I>> and
        std::movable<I>;

    template <typename Rng, typename F>
    concept for_eachable_range =
        input_range<Rng> and
        for_eachable_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::actions {
    struct for_each_fn {
        template <typename Rng, typename F>
        requires details::concepts::for_eachable_range<Rng, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f) const -> Rng& {
            for (auto it = std::begin(rng), end = std::end(rng); it != end; ++it) { std::invoke(f, *it); }
            return rng;
        }

        template <typename F>
        GENEX_INLINE constexpr auto operator()(F f) const noexcept -> auto {
            return std::bind_back(for_each_fn{}, f);
        }
    };

    inline constexpr for_each_fn for_each{};
}
