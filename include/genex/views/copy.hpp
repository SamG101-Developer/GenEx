#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept copyable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::copy_constructible<iter_value_t<I>> and
        std::convertible_to<iter_reference_t<I>, iter_value_t<I>>;

    template <typename Rng>
    concept copyable_range =
        input_range<Rng> and
        copyable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::copyable_iters<I, S>
    auto do_copy(I first, S last) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct copy_fn {
        template <typename I, typename S>
            requires concepts::copyable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_copy(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::copyable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(copy_fn{});
        }
    };

    EXPORT_GENEX_STRUCT(copy);
}
