#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename F>
    concept intersperable_with_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::invocable<F> and
        std::convertible_to<std::invoke_result_t<F>, iter_value_t<I>> and
        std::common_with<iter_value_t<I>, std::invoke_result_t<F>>;

    template <typename Rng, typename F>
    concept intersperable_with_range =
        input_range<Rng> and
        intersperable_with_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::views::detail {
    template <typename I, typename S, std::invocable F>
        requires concepts::intersperable_with_iters<I, S, F>
    GENEX_NO_ASAN
    auto do_intersperse_with(I first, S last, F &&sep) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        co_yield *first;
        ++first;
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(sep));
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct intersperse_with_fn {
        template <typename I, typename S, typename F>
            requires concepts::intersperable_with_iters<I, S, F>
        constexpr auto operator()(I first, S last, F &&sep) const -> auto {
            return detail::do_intersperse_with(
                std::move(first), std::move(last), std::forward<F>(sep));
        }

        template <typename Rng, typename F>
            requires concepts::intersperable_with_range<Rng, F>
        constexpr auto operator()(Rng &&rng, F &&sep) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<F>(sep));
        }

        template <typename F>
            requires (not range<F>)
        constexpr auto operator()(F &&sep) const -> auto {
            return std::bind_back(
                intersperse_with_fn{}, std::forward<F>(sep));
        }
    };

    GENEX_EXPORT_STRUCT(intersperse_with);
}
