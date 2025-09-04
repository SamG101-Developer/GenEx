#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_forward_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        std::movable<std::remove_reference_t<iter_reference_t<I>>>;

    template <typename Rng>
    concept can_forward_range =
        input_range<Rng> and
        can_forward_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::can_forward_iters<I, S>
    auto do_forward(I first, S last) -> generator<std::remove_reference_t<iter_reference_t<I>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::move(*first);
        }
    }
}


namespace genex::views {
    struct forward_fn {
        template <typename I, typename S>
            requires concepts::can_forward_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_forward(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::can_forward_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(forward_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(forward);
}
