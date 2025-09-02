#pragma once
#include <iterator>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <std::size_t N, typename I, typename S>
    concept can_tuple_element_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        tuple_like<iter_value_t<I>> and
        N <= std::tuple_size_v<iter_value_t<I>>;

    template <std::size_t N, typename Rng>
    concept can_tuple_element_range =
        std::ranges::input_range<Rng> and
        can_tuple_element_iters<N, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <std::size_t N, typename I, typename S>
    auto do_tuple_n(I first, S last) -> generator<std::tuple_element_t<N, iter_value_t<I>>> requires concepts::can_tuple_element_iters<N, I, S> {
        for (; first != last; ++first) {
            co_yield std::get<N>(*first);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(tuple_element_) {
        template <std::size_t N, typename I, typename S> requires concepts::can_tuple_element_iters<N, I, S>
        auto operator()(I first, S last) const -> auto {
            // Call the tuple_n inner function.
            auto gen = detail::do_tuple_n<N>(std::move(first), std::move(last));
            return tuple_element__view(std::move(gen));
        }

        template <std::size_t N, typename Rng> requires concepts::can_tuple_element_range<N, Rng>
        auto operator()(Rng &&rng) const -> auto {
            // Call the take inner function.
            return this->operator()<N>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <std::size_t N>
        auto operator()() const -> auto {
            // Create a closure that takes a range and applies the take.
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_tuple_element_range<N, Rng>
                (Rng &&rng) mutable -> auto {
                return this->operator()<N>(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(tuple_element_);
}


#define tuple_element tuple_element_.operator()
