module;
#include <genex/macros.hpp>

export module genex.views2.view;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.span;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept viewable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept viewable_range =
        input_range<Rng> and
        viewable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views {
    struct view_fn {
        template <typename I, typename S>
        requires detail::concepts::viewable_iters<I, S> and std::random_access_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return genex::span<iter_value_t<I>>(std::move(first), std::move(last));
        }

        template <typename I, typename S>
        requires detail::concepts::viewable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return std::ranges::subrange<iter_value_t<I>>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::viewable_range<Rng> and std::random_access_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return genex::span<range_value_t<Rng>>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::viewable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return std::ranges::subrange<range_value_t<Rng>>(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(view_fn{});
        }
    };

    export inline constexpr view_fn view{};
}
