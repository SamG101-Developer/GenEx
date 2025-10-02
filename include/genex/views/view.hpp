#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_view_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept can_view_range =
        input_range<Rng> and
        can_view_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::can_view_iters<I, S>
    GENEX_NO_ASAN
    auto do_view(I first, S last) -> generator<iter_value_t<I>> {
        for (auto it = first; it != last; ++it) {
            co_yield static_cast<iter_value_t<I>>(*it);
        }
    }
}


namespace genex::views {
    struct view_fn {
        template <typename I, typename S>
            requires concepts::can_view_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_view(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::can_view_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(view_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(view);
}
