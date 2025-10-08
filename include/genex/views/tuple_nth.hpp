#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <std::size_t N, typename I, typename S>
    concept tuple_indexable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        tuple_like<iter_value_t<I>> and
        N <= std::tuple_size_v<iter_value_t<I>>;

    template <std::size_t N, typename Rng>
    concept tuple_indexable_range =
        std::ranges::input_range<Rng> and
        tuple_indexable_iters<N, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::coros {
    template <std::size_t N, typename I, typename S>
    requires concepts::tuple_indexable_iters<N, I, S>
    auto do_tuple_nth(I first, S last) -> generator<std::tuple_element_t<N, iter_value_t<I>>> {
        for (; first != last; ++first) {
            co_yield std::get<N>(*first);
        }
    }
}


namespace genex::views {
    template <std::size_t N>
    struct tuple_nth_fn {
        template <typename I, typename S>
        requires detail::concepts::tuple_indexable_iters<N, I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::coros::do_tuple_nth<N>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::tuple_indexable_range<N, Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_tuple_nth<N>(std::move(first), std::move(last));
        }
        
        GENEX_INLINE auto operator()() const {
            return std::bind_back(tuple_nth_fn{});
        }
    };

    template <std::size_t N>
    inline constexpr tuple_nth_fn<N> tuple_nth{};
}
