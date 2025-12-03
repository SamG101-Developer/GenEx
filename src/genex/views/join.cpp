module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.join;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept joinable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>>;

    template <typename Rng>
    concept joinable_range =
        input_range<Rng> and
        joinable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
    auto do_join(I first, S last) -> generator<range_value_t<iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            for (auto &&x : *first) {
                co_yield static_cast<range_value_t<iter_value_t<I>>>(x);
            }
        }
    }
}


namespace genex::views {
    struct join_fn {
        template <typename I, typename S>
        requires detail::concepts::joinable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_join(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::joinable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_join(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(join_fn{});
        }
    };

    export inline constexpr join_fn join{};
}
