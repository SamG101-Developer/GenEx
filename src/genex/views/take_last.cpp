module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.take_last;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.views.drop;
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


namespace genex::views::detail::impl {
    template <typename I, typename S, typename Int>
    requires concepts::takeable_last_iters<I, S, Int>
    auto do_take_last(I first, S last, const Int n) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        auto size = iterators::distance(first, last);
        for (auto x: views::drop(std::move(first), std::move(last), size > n ? size - n : 0)) {
            co_yield std::move(x);
        }
    }
}


namespace genex::views {
    struct take_last_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::takeable_last_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, Int n) const {
            return detail::impl::do_take_last(std::move(first), std::move(last), std::move(n));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::takeable_last_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_take_last(std::move(first), std::move(last), std::move(n));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(Int n) const {
            return meta::bind_back(take_last_fn{}, std::move(n));
        }
    };

    export inline constexpr take_last_fn take_last{};
}
