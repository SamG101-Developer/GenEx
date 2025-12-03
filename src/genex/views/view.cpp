module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.view;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept can_view_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept can_view_range =
        input_range<Rng> and
        can_view_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::can_view_iters<I, S>
    auto do_view(I first, S last) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (auto it = first; it != last; ++it) {
            co_yield static_cast<iter_value_t<I>>(*it);
        }
    }
}


namespace genex::views {
    struct view_fn {
        template <typename I, typename S>
        requires detail::concepts::can_view_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::can_view_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(view_fn{});
        }
    };

    export inline constexpr view_fn view{};
}
