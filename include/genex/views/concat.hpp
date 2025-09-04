#pragma once
#include <coroutine>
#include <tuple>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
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


namespace genex::views::detail {
    template <typename I1, typename S1>
        requires concepts::concatenatable_iters<std::tuple<I1>, std::tuple<S1>>
    auto do_concat(I1 first1, S1 last1) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            co_yield static_cast<std::common_type_t<iter_value_t<I1>, iter_value_t<I1>>>(*first1);
        }
    }

    template <typename I1, typename S1, typename... Is, typename... Ss>
        requires (concepts::concatenatable_iters<std::tuple<I1, Is...>, std::tuple<S1, Ss...>> and sizeof...(Is) == sizeof...(Ss))
    auto do_concat(I1 first1, S1 last1, std::tuple<Is...> firsts, std::tuple<Ss...> lasts) -> generator<std::common_type_t<iter_value_t<I1>, iter_value_t<Is>...>> {
        for (auto gen = do_concat(std::move(first1), std::move(last1)); auto &&x : gen) {
            co_yield std::forward<decltype(x)>(x);
        }
        if constexpr (sizeof...(Is) > 0) {
            for (auto rest = do_concat(std::move(std::get<0>(firsts)), std::move(std::get<0>(lasts)), tuple_tail(std::move(firsts)), tuple_tail(std::move(lasts))); auto &&x : rest) { co_yield std::forward<decltype(x)>(x); }
        }
    }
}


namespace genex::views {
    struct concat_fn {
        template <typename... Is, typename... Ss>
            requires concepts::concatenatable_iters<std::tuple<Is...>, std::tuple<Ss...>>
        constexpr auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const -> auto {
            return detail::do_concat(
                std::move(std::get<0>(first)), std::move(std::get<0>(last)),
                tuple_tail(std::move(first)), tuple_tail(std::move(last)));
        }

        template <typename... Rngs>
            requires (concepts::concatenatable_range<Rngs...> and sizeof...(Rngs) > 1)
        constexpr auto operator()(Rngs &&... ranges) const -> auto {
            return (*this)(
                std::make_tuple(iterators::begin(std::forward<Rngs>(ranges))...),
                std::make_tuple(iterators::end(std::forward<Rngs>(ranges))...));
        }

        template <typename Rng2>
            requires concepts::concatenatable_range<Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return std::bind_back(concat_fn{}, std::forward<Rng2>(rng2));
        }
    };

    GENEX_EXPORT_STRUCT(concat);
}
