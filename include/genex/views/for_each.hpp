#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
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


namespace genex::views::detail {
    template <typename I, typename S, typename F>
        requires concepts::for_eachable_iters<I, S, F>
    auto do_for_each(I first, S last, F &&f) -> void {
        if (first == last) { return; }
        for (; first != last; ++first) {
            std::invoke(std::forward<F>(f), std::forward<decltype(*first)>(*first));
        }
    }
}


namespace genex::views {
    struct for_each_fn {
        template <typename I, typename S, typename F>
            requires concepts::for_eachable_iters<I, S, F>
        constexpr auto operator()(I first, S last, F &&f) const -> void {
            detail::do_for_each(std::move(first), std::move(last), std::forward<F>(f));
        }

        template <typename Rng, typename F>
            requires concepts::for_eachable_range<Rng, F>
        constexpr auto operator()(Rng &&rng, F &&f) const -> void {
            auto [first, last] = iterators::iter_pair(rng);
            (*this)(std::move(first), std::move(last), std::forward<F>(f));
        }

        template <typename F>
            requires (not range<F>)
        constexpr auto operator()(F &&f) const -> auto {
            return std::bind_back(for_each_fn{}, std::forward<F>(f));
        }
    };

    EXPORT_GENEX_STRUCT(for_each);
}
