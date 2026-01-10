module;
#include <genex/macros.hpp>

export module genex.views2.for_each;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename F>
    concept for_eachable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::invocable<F, iter_reference_t<I>>;

    template <typename Rng, typename F>
    concept for_eachable_range =
        input_range<Rng> and
        for_eachable_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::views2 {
    struct for_each_fn {
        template <typename I, typename S, typename F>
        requires detail::concepts::for_eachable_iters<I, S, F>
        GENEX_INLINE constexpr auto operator()(I first, S last, F f) const {
            for (; first != last; ++first) { meta::invoke(f, *first); }
        }

        template <typename Rng, typename F>
        requires detail::concepts::for_eachable_range<Rng, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f) const {
            for (auto &it : rng) { meta::invoke(f, it); }
        }

        template <typename F>
        requires (not range<F>)
        GENEX_INLINE constexpr auto operator()(F f) const {
            return meta::bind_back(for_each_fn{}, std::move(f));
        }
    };

    export inline constexpr for_each_fn for_each{};
}
