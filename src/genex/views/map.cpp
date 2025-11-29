module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.map;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept keyable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        pair_like<iter_reference_t<I>>;

    template <typename I, typename S>
    concept valable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        pair_like<iter_reference_t<I>>;

    template <typename Rng>
    concept keyable_range =
        input_range<Rng> and
        keyable_iters<iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename Rng>
    concept valable_range =
        input_range<Rng> and
        valable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::keyable_iters<I, S>
    auto do_keys(I first, S last) -> generator<typename iter_value_t<I>::first_type> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield std::get<0>(*first);
        }
    }

    template <typename I, typename S>
    requires concepts::valable_iters<I, S>
    auto do_vals(I first, S last) -> generator<typename iter_value_t<I>::second_type> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield std::get<1>(*first);
        }
    }
}


namespace genex::views {
    struct keys_fn {
        template <typename I, typename S>
        requires detail::concepts::keyable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_keys(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::keyable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_keys(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(keys_fn{});
        }
    };

    struct vals_fn {
        template <typename I, typename S>
        requires detail::concepts::valable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_vals(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::valable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_vals(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(vals_fn{});
        }
    };

    export inline constexpr keys_fn keys{};
    export inline constexpr vals_fn vals{};
}
