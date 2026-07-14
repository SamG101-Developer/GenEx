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

namespace genex::views::detail::concepts {
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

namespace genex::views::detail::impl {
    // A sentinel for a counted range that stops at whichever comes first: the count running out,
    // or the underlying range reaching its end. Unlike a two-iterator subrange, this does not rely
    // on the base iterator's equality reflecting distance travelled, so it works for ranges whose
    // iterators wrap (e.g. cycle) where advancing can revisit an earlier base position.
    template <typename S>
    struct take_sentinel {
        GENEX_NO_UNIQUE_ADDRESS S st;

        template <typename I>
        GENEX_INLINE friend constexpr auto operator==(const std::counted_iterator<I> &it, const take_sentinel &self) -> bool {
            return it.count() == 0 or it.base() == self.st;
        }
    };
}

namespace genex::views {
    struct take_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_iters<I, S, Int> and std::contiguous_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
            SAFE_CTOR(genex::span<iter_value_t<I>>, I, I) and SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
            return genex::span<iter_value_t<I>>(std::move(first), std::move(first) + static_cast<std::ptrdiff_t>(n));
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const noexcept(
            SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Int)) {
            return std::ranges::subrange(
                std::counted_iterator(std::move(first), static_cast<iter_difference_t<I>>(n)),
                detail::impl::take_sentinel<S>{std::move(last)});
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int> and std::contiguous_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
            SAFE_CTOR(genex::span<range_value_t<Rng>>, iterator_t<Rng>, iterator_t<Rng>) and SAFE_MOVE(Int)) {
            auto [first, last] = iterators::iter_pair(rng);
            return genex::span<range_value_t<Rng>>(std::move(first), std::move(first) + static_cast<std::ptrdiff_t>(n));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const noexcept(
            SAFE_MOVE(iterator_t<Rng>) and SAFE_MOVE(sentinel_t<Rng>) and SAFE_MOVE(Int)) {
            auto [first, last] = iterators::iter_pair(rng);
            return std::ranges::subrange(
                std::counted_iterator(std::move(first), static_cast<iter_difference_t<iterator_t<Rng>>>(n)),
                detail::impl::take_sentinel<sentinel_t<Rng>>{std::move(last)});
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const noexcept(
            SAFE_CTOR(take_fn) and SAFE_MOVE(Int)) {
            return meta::bind_back(take_fn{}, std::move(n));
        }
    };

    export inline constexpr take_fn take{};
}
