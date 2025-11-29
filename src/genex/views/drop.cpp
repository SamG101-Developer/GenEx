module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.drop;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept droppable_range =
        input_range<Rng> and
        droppable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S, typename Int>
    requires concepts::droppable_iters<I, S, Int> and std::random_access_iterator<I>
    auto do_drop(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        auto true_first = first + std::min<std::size_t>(static_cast<std::size_t>(n), iterators::distance(first, last));
        for (; true_first != last; ++true_first) {
            co_yield *true_first;
        }
    }

    template <typename I, typename S, typename Int>
    requires concepts::droppable_iters<I, S, Int>
    auto do_drop(I first, S last, Int n) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (; n > 0 and first != last; --n, ++first);
        for (; first != last; ++first) {
            co_yield *first;
        }
    }
}


namespace genex::views {
    struct drop_fn {
        template <typename I, typename S, typename Int>
            requires detail::concepts::droppable_iters<I, S, Int>
        GENEX_INLINE auto operator()(I first, S last, const Int n) const {
            return detail::impl::do_drop(std::move(first), std::move(last), std::move(n));
        }

        template <typename Rng, typename Int>
            requires detail::concepts::droppable_range<Rng, Int>
        GENEX_INLINE auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_drop(std::move(first), std::move(last), std::move(n));
        }

        template <typename Int>
            requires std::weakly_incrementable<Int>
        GENEX_INLINE auto operator()(Int n) const {
            return meta::bind_back(drop_fn{}, std::move(n));
        }
    };

    export inline constexpr drop_fn drop{};
}
