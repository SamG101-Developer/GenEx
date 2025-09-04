#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename New>
    concept interspersable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::constructible_from<New, iter_value_t<I>> and
        std::common_with<iter_value_t<I>, New> and
        std::copy_constructible<New>;

    template <typename Rng, typename New>
    concept interspersable_range =
        input_range<Rng> and
        interspersable_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename New>
        requires concepts::interspersable_iters<I, S, New>
    auto do_intersperse(I first, S last, New &&sep) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        co_yield *first;
        ++first;
        for (; first != last; ++first) {
            co_yield sep;
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct intersperse_fn {
        template <typename I, typename S, typename New>
            requires concepts::interspersable_iters<I, S, New>
        constexpr auto operator()(I first, S last, New &&sep) const -> auto {
            return detail::do_intersperse(
                std::move(first), std::move(last), std::forward<New>(sep));
        }

        template <typename Rng, typename New>
            requires concepts::interspersable_range<Rng, New>
        constexpr auto operator()(Rng &&rng, New &&sep) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<New>(sep));
        }

        template <typename New>
        constexpr auto operator()(New &&sep) const -> auto {
            return std::bind_back(intersperse_fn{}, std::forward<New>(sep));
        }
    };

    EXPORT_GENEX_STRUCT(intersperse);
}
