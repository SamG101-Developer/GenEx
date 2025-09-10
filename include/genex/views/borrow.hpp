#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept borrowable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;;

    template <typename Rng>
    concept borrowable_range =
        input_range<Rng> and
        borrowable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::borrowable_iters<I, S>
    GENEX_NO_ASAN
    auto do_borrow(I first, S last) -> generator<iter_value_t<I>&> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    struct borrow_fn {
        template <typename I, typename S>
            requires concepts::borrowable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_borrow(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::borrowable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(borrow_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(borrow);
}
