#pragma once
#include <coroutine>
#include <tuple>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename T1, typename T2>
    struct can_concat_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct can_concat_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (std::input_iterator<Is> and ...) and
            (std::sentinel_for<Ss, Is> and ...) and
            (std::movable<Is> and ...) and
            requires { typename std::common_type_t<iter_value_t<Is>...>; };
    };

    template <typename T1, typename T2>
    concept can_concat_iters =
        can_concat_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept can_concat_range =
        (input_range<Rngs> and ...) and
        can_concat_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}


namespace genex::views::detail {
    /* RANGE IMPLEMENTATION:
     *
    template <typename Rng1, typename Rng2>
    auto do_concat(Rng1 &&rng1, Rng2 &&rng2) -> generator<range_value_t<Rng1>> {
        for (auto &&x : rng1) { co_yield std::forward<decltype(x)>(x); }
        for (auto &&x : rng2) { co_yield std::forward<decltype(x)>(x); }
    }

    template <typename Rng1, typename Rng2, typename... Rngs>
    auto do_concat(Rng1 &&rng1, Rng2 &&rng2, Rngs &&... rngs) -> generator<range_value_t<Rng1>> {
        for (auto &&x : rng1) { co_yield std::forward<decltype(x)>(x); }
        auto rest = do_concat(std::forward<Rng2>(rng2), std::forward<Rngs>(rngs)...);
        for (auto &&x : rest) { co_yield std::forward<decltype(x)>(x); }
    }

    * NEED TO CONVERT TO ITERATOR IMPLEMENTATION:
    */

    template <typename I1, typename S1>
    auto do_concat(I1 first1, S1 last1) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) { co_yield static_cast<std::common_type_t<iter_value_t<I1>, iter_value_t<I1>>>(*first1); }
    }

    template <typename I1, typename S1, typename... Is, typename... Ss> requires (sizeof...(Is) == sizeof...(Ss))
    auto do_concat(I1 first1, S1 last1, std::tuple<Is...> firsts, std::tuple<Ss...> lasts) -> generator<std::common_type_t<iter_value_t<I1>, iter_value_t<Is>...>> {
        for (auto gen = do_concat(std::move(first1), std::move(last1)); auto &&x : gen) { co_yield std::forward<decltype(x)>(x); }
        if constexpr (sizeof...(Is) > 0) {
            for (auto rest = do_concat(std::move(std::get<0>(firsts)), std::move(std::get<0>(lasts)), tuple_tail(std::move(firsts)), tuple_tail(std::move(lasts))); auto &&x : rest) { co_yield std::forward<decltype(x)>(x); }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(concat) {
        template <typename... Is, typename... Ss> requires concepts::can_concat_iters<std::tuple<Is...>, std::tuple<Ss...>>
        auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const -> auto {
            auto gen = detail::do_concat(
                std::move(std::get<0>(first)), std::move(std::get<0>(last)),
                tuple_tail(std::move(first)), tuple_tail(std::move(last)));
            return concat_view(std::move(gen));
        }

        template <typename... Rngs> requires (concepts::can_concat_range<Rngs...> and sizeof...(Rngs) > 1)
        auto operator()(Rngs &&... ranges) const -> auto {
            return (*this)(std::make_tuple(iterators::begin(std::forward<Rngs>(ranges))...), std::make_tuple(iterators::end(std::forward<Rngs>(ranges))...));
        }

        template <typename Rng2> requires concepts::can_concat_range<Rng2>
        auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires concepts::can_concat_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    EXPORT_GENEX_VIEW(concat);
}
