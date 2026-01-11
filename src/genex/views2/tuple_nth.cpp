module;
#include <genex/macros.hpp>

export module genex.views2.tuple_nth;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.algorithms.tuple;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.views2.transform;
import std;


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


namespace genex::views {
    template <std::size_t N>
    struct tuple_nth_fn {
        template <typename I, typename S>
        requires detail::concepts::tuple_indexable_iters<N, I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return transform(std::move(first), std::move(last), genex::get<N>);
        }

        template <typename Rng>
        requires detail::concepts::tuple_indexable_range<N, Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return transform(std::move(first), std::move(last), genex::get<N>);
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(tuple_nth_fn{});
        }
    };

    export template <std::size_t N>
    inline constexpr tuple_nth_fn<N> tuple_nth{};
}
