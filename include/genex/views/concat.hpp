#pragma once
#include <tuple>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename T1, typename T2>
    struct concatenatable_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct concatenatable_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (std::input_iterator<Is> and ...) and
            (std::sentinel_for<Ss, Is> and ...) and
            requires { typename std::common_type_t<iter_value_t<Is>...>; };
    };

    template <typename T1, typename T2>
    concept concatenatable_iters =
        concatenatable_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept concatenatable_range =
        (input_range<Rngs> and ...) and
        concatenatable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}


namespace genex::views::detail::impl {
    template <typename Rng>
    requires concepts::concatenatable_range<Rng>
    auto do_concat(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (decltype(auto) x : rng) {
            co_yield x;
        }
    }

    template <typename Rng, typename... Rngs>
    requires concepts::concatenatable_range<Rng, Rngs...> and (sizeof...(Rngs) > 0)
    auto do_concat(Rng &&rng, Rngs &&... rngs) -> generator<range_value_t<Rng>> {
        for (decltype(auto) x : rng) {
            co_yield x;
        }
        for (decltype(auto) x : do_concat(std::forward<Rngs>(rngs)...)) {
            co_yield x;
        }
    }
}


namespace genex::views {
    struct concat_fn {
        template <typename... Rngs>
        requires (detail::concepts::concatenatable_range<Rngs...> and sizeof...(Rngs) > 1)
        GENEX_INLINE constexpr auto operator()(Rngs &&... ranges) const {
            return detail::impl::do_concat(std::forward<Rngs>(ranges)...);
        }

        template <typename Rng2>
        requires detail::concepts::concatenatable_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
            return meta::bind_back(concat_fn{}, std::forward<Rng2>(rng2));
        }
    };

    inline constexpr concat_fn concat{};
}
