module;
#include <genex/macros.hpp>

export module genex.views2.map;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import genex.algorithms.tuple;
import genex.views2.transform;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept mappable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        pair_like<iter_reference_t<I>>;

    template <typename Rng>
    concept mappable_range =
        input_range<Rng> and
        mappable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views {
    template <std::size_t N>
    struct map_fn {
        template <typename I, typename S>
        requires detail::concepts::mappable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return transform(std::move(first), std::move(last), genex::get<N>);
        }

        template <typename Rng>
        requires detail::concepts::mappable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return transform(std::move(first), std::move(last), genex::get<N>);
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(map_fn{});
        }
    };

    export inline constexpr map_fn<0> keys{};
    export inline constexpr map_fn<1> vals{};
}

