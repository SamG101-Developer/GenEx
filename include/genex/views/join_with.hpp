#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/next.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename New>
    concept joinable_with_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::convertible_to<New, range_value_t<iter_value_t<I>>>;

    template <typename Rng, typename New>
    concept joinable_with_range =
        forward_range<Rng> and
        joinable_with_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
    auto do_join_with(I first, S last, New sep) -> generator<range_value_t<iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            for (auto &&x : *first) { co_yield static_cast<range_value_t<iter_value_t<I>>>(x); }
            if (iterators::next(first) != last) { co_yield sep; }
        }
    }
}


namespace genex::views {
    struct join_with_fn {
        template <typename I, typename S, typename New>
        requires detail::concepts::joinable_with_iters<I, S, New>
        GENEX_INLINE constexpr auto operator()(I first, S last, New sep) const {
            return detail::impl::do_join_with(std::move(first), std::move(last), std::move(sep));
        }

        template <typename Rng, typename New>
        requires detail::concepts::joinable_with_range<Rng, New>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New sep) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_join_with(std::move(first), std::move(last), std::move(sep));
        }

        template <typename New>
        GENEX_INLINE constexpr auto operator()(New sep) const {
            return meta::bind_back(join_with_fn{}, std::move(sep));
        }
    };

    inline constexpr join_with_fn join_with{};
}
