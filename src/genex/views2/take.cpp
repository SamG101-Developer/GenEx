module;
#include <genex/macros.hpp>

export module genex.views2.take;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.span;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename Int>
    concept takeable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept takeable_range =
        input_range<Rng> and
        takeable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}



namespace genex::views2 {
    struct take_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_iters<I, S, Int> and std::random_access_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            // todo: min
            return genex::span<iter_value_t<I>>(std::move(first), std::move(first) + n);
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            auto it = std::move(first);
            for (Int i = 0; i < n and it != last; ++i) {
                it = iterators::next(std::move(it));
            }
            return std::ranges::subrange(std::move(first), std::move(it));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int> and std::random_access_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            // todo: min
            auto [first, last] = iterators::iter_pair(rng);
            return genex::span<range_value_t<Rng>>(std::move(first), std::move(first) + n);
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            auto it = std::move(first);
            for (Int i = 0; i < n and it != last; ++i) {
                it = iterators::next(std::move(it));
            }
            return std::ranges::subrange(std::move(first), std::move(it));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            return meta::bind_back(take_fn{}, std::move(n));
        }
    };

    export inline constexpr take_fn take{};
}
