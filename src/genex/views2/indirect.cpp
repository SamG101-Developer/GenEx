module;
#include <genex/macros.hpp>

export module genex.views2.indirect;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.views2.transform;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S>
    concept indirectable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        requires(I it) { { **it }; };

    template <typename Rng>
    concept indirectable_range =
        input_range<Rng> and
        indirectable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}



namespace genex::views2 {
    struct indirect_fn {
        template <typename I, typename S>
        requires detail::concepts::indirectable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return transform(std::move(first), std::move(last), [](auto &&ptr) { return *ptr; });
        }

        template <typename Rng>
        requires detail::concepts::indirectable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return transform(std::move(first), std::move(last), [](auto &&ptr) { return *ptr; });
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(indirect_fn{});
        }
    };

    export inline constexpr indirect_fn indirect{};
}
