module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.take;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
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
    template <typename I, typename S, typename Int>
    requires concepts::takeable_iters<I, S, Int> and std::random_access_iterator<I>
    auto do_take(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        auto true_last = first + std::min<Int>(n, iterators::distance(first, last));
        for (; first != true_last; ++first) {
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Int>
    requires concepts::takeable_iters<I, S, Int>
    auto do_take(I first, S last, Int n) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (; n > 0 and first != last; --n, ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    struct take_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, Int n) const {
            return detail::impl::do_take(std::move(first), std::move(last), std::move(n));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_take(std::move(first), std::move(last), std::move(n));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            return meta::bind_back(take_fn{}, std::move(n));
        }
    };

    export inline constexpr take_fn take{};
}
