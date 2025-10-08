#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename To, typename I, typename S>
    concept static_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        requires(iter_value_t<I> &&from) { static_cast<To>(from); };

    template <typename To, typename Rng>
    concept static_castable_range =
        input_range<Rng> and
        static_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::coros {
    template <typename To, typename I, typename S>
    requires concepts::static_castable_iters<To, I, S>
    auto do_static_cast_iter(I first, S last) -> generator<To> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield static_cast<To>(*first);
        }
    }
}


namespace genex::views {
    template <typename To>
    struct cast_static_fn {
        template <typename I, typename S>
        requires detail::concepts::static_castable_iters<To, I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::coros::do_static_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::static_castable_range<To, Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_static_cast_iter<To>(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(cast_static_fn{});
        }
    };

    template <typename To>
    inline constexpr cast_static_fn<To> cast_static{};
}
