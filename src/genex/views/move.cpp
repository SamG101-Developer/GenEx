module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.move;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept movable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>>;

    template <typename Rng>
    concept movable_range =
        input_range<Rng> and
        movable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::movable_iters<I, S>
    auto do_move(I first, S last) -> generator<iter_value_t<I>&&> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield std::move(*first);
        }
    }
}


namespace genex::views {
    struct move_fn {
        template <typename I, typename S>
        requires detail::concepts::movable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_move(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::movable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_move(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(move_fn{});
        }
    };

    export inline constexpr move_fn move{};
}
