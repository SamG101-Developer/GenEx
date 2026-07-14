module;
#include <genex/macros.hpp>

export module genex.views2.take_last;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.span;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.next;
import std;

namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept takeable_last_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept takeable_last_range =
        input_range<Rng> and
        takeable_last_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}

namespace genex::views {
    struct take_last_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_last_iters<I, S, Int> and std::contiguous_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
            SAFE_CTOR(genex::span<iter_value_t<I>>, I, S) and
            SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
            return genex::span<iter_value_t<I>>(std::move(last) - n, std::move(last));
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_last_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
            SAFE_CTOR(std::ranges::subrange<I, S>, I, S) and
            SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
            auto dist = iterators::distance(first, last);
            return std::ranges::subrange(iterators::next(std::move(first), dist - n), std::move(last));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_last_range<Rng, Int> and std::contiguous_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
            SAFE_CTOR(genex::span<range_value_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>) and
            SAFE_MOVE(Int)) {
            auto [first, last] = iterators::iter_pair(rng);
            return genex::span<range_value_t<Rng>>(std::move(last) - n, std::move(last));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_last_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
            SAFE_CTOR(std::ranges::subrange<iterator_t<Rng>, sentinel_t<Rng>>, iterator_t<Rng>, sentinel_t<Rng>) and
            SAFE_MOVE(Int)) {
            auto [first, last] = iterators::iter_pair(rng);
            auto dist = iterators::distance(first, last);
            return std::ranges::subrange(iterators::next(std::move(first), dist - n), std::move(last));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
            SAFE_CTOR(take_last_fn) and SAFE_MOVE(Int)) {
            return meta::bind_back(take_last_fn{}, std::move(n));
        }
    };

    export inline constexpr take_last_fn take_last{};
}
