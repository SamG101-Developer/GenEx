module;
#include <genex/macros.hpp>

export module genex.views2.drop_last;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.span;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import genex.iterators.prev;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept droppable_last_range =
        input_range<Rng> and
        droppable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views2 {
    struct drop_last_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_last_iters<I, S, Int> and std::random_access_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            return genex::span<iter_value_t<I>>(std::move(first), std::move(last) - n);
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_last_iters<I, S, Int> and std::bidirectional_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            return std::ranges::subrange(std::move(first), iterators::prev(std::move(last), n));
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_last_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            // todo: slow
            return std::ranges::subrange(std::move(first), iterators::next(std::move(first), iterators::distance(first, last) - n));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_last_range<Rng, Int> and std::random_access_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return genex::span<range_value_t<Rng>>(std::move(first), std::move(last) - n);
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_last_range<Rng, Int> and std::bidirectional_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return std::ranges::subrange(std::move(first), iterators::prev(std::move(last), n));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_last_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            // todo: slow
            auto [first, last] = iterators::iter_pair(rng);
            return std::ranges::subrange(std::move(first), iterators::next(std::move(first), iterators::distance(first, last) - n));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            return meta::bind_back(drop_last_fn{}, std::move(n));
        }
    };

    export inline constexpr drop_last_fn drop_last{};
}
