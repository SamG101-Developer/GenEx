#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept enumerable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept enumerable_range =
        input_range<Rng> and
        enumerable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::enumerable_iters<I, S>
    GENEX_NO_ASAN
    auto do_enumerate(I first, S last) -> generator<std::pair<std::size_t, iter_value_t<I>>> {
        if (first == last) { co_return; }
        auto i = 0;
        for (; first != last; ++first) {
            co_yield {i++, static_cast<iter_value_t<I>>(*first)};
        }
    }
}


namespace genex::views {
    struct enumerate_fn {
        template <typename I, typename S>
            requires concepts::enumerable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_enumerate(std::move(first), std::move(last));
        }

        template <typename Rng> requires concepts::enumerable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(enumerate_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(enumerate);
}
