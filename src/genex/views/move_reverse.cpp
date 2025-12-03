module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.move_reverse;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import genex.iterators.prev;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept movable_reversible_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>>;

    template <typename Rng>
    concept movable_reversible_range =
        bidirectional_range<Rng> and
        movable_reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::movable_reversible_iters<I, S>
    auto do_move_reverse(I first, S last) -> generator<iter_value_t<I>&&> {
        GENEX_ITER_GUARD;
        for (; first != last; --last) {
            co_yield std::move(*iterators::prev(last, 1, first));
        }
    }
}


namespace genex::views {
    struct move_reverse_fn {
        template <typename I, typename S>
        requires detail::concepts::movable_reversible_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_move_reverse(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::movable_reversible_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_move_reverse(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(move_reverse_fn{});
        }
    };

    export inline constexpr move_reverse_fn move_reverse{};
}
