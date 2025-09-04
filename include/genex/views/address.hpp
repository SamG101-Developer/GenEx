#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept addressable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        requires(I it) { { std::addressof(*it) }; };

    template <typename Rng>
    concept addressable_range =
        input_range<Rng> and
        addressable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::addressable_iters<I, S>
    auto do_address(I first, S last) -> generator<std::add_pointer_t<std::remove_reference_t<decltype(*std::declval<I&>())>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::addressof(*first);
        }
    }
}


namespace genex::views {
    struct address_fn {
        template <typename I, typename S>
            requires concepts::addressable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_address(
                std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::addressable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last));
        }

        auto operator()() const -> auto {
            return std::bind_back(address_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(address);
}
