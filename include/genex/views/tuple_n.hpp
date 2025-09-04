#pragma once
#include <iterator>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <std::size_t N, typename I, typename S>
    concept can_tuple_element_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        tuple_like<iter_value_t<I>> and
        N <= std::tuple_size_v<iter_value_t<I>>;

    template <std::size_t N, typename Rng>
    concept can_tuple_element_range =
        std::ranges::input_range<Rng> and
        can_tuple_element_iters<N, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <std::size_t N, typename I, typename S>
        requires concepts::can_tuple_element_iters<N, I, S>
    auto do_tuple_n(I first, S last) -> generator<std::tuple_element_t<N, iter_value_t<I>>> {
        for (; first != last; ++first) {
            co_yield std::get<N>(*first);
        }
    }
}


namespace genex::views {
    struct tuple_element_impl_fn {
        template <std::size_t N, typename I, typename S>
            requires concepts::can_tuple_element_iters<N, I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_tuple_n<N>(std::move(first), std::move(last));
        }

        template <std::size_t N, typename Rng> requires concepts::can_tuple_element_range<N, Rng>
        auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return this->operator()<N>(std::move(first), std::move(last));
        }

        template <std::size_t N>
        auto operator()() const -> auto {
            return [*this]<typename Rng> requires concepts::can_tuple_element_range<N, Rng>(Rng &&rng) {
                return this->operator()<N>(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_STRUCT(tuple_element_impl);
}


#define tuple_element tuple_element_impl.operator()
