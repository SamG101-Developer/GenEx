#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept keyable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        pair_like<iter_reference_t<I>>;

    template <typename I, typename S>
    concept valable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        pair_like<iter_reference_t<I>>;

    template <typename Rng>
    concept keyable_range =
        input_range<Rng> and
        keyable_iters<iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename Rng>
    concept valable_range =
        input_range<Rng> and
        valable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::keyable_iters<I, S>
    GENEX_NO_ASAN
    auto do_keys(I first, S last) -> generator<typename iter_value_t<I>::first_type> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::get<0>(*first);
        }
    }

    template <typename I, typename S>
        requires concepts::valable_iters<I, S>
    GENEX_NO_ASAN
    auto do_vals(I first, S last) -> generator<typename iter_value_t<I>::second_type> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::get<1>(*first);
        }
    }
}


namespace genex::views {
    struct keys_fn {
        template <typename I, typename S>
            requires concepts::keyable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_keys(std::move(first), std::move(last));
        }

        template <typename Rng> requires concepts::keyable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(keys_fn{});
        }
    };

    struct vals_fn {
        template <typename I, typename S>
            requires concepts::valable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_vals(std::move(first), std::move(last));
        }

        template <typename Rng> requires concepts::valable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(vals_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(keys);
    GENEX_EXPORT_STRUCT(vals);
}
