#pragma once
#include <coroutine>
#include <tuple>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I1, typename S1, typename I2, typename S2>
    auto do_concat(I1 first1, S1 last1, I2 first2, S2 last2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) { co_yield *first1; }
        for (; first2 != last2; ++first2) { co_yield *first2; }
    }

    template <typename... Is, typename... Ss>
    auto do_concat(std::tuple<Is...> &&first, std::tuple<Ss...> &&last) -> generator<iter_value_t<std::tuple_element_t<0, std::tuple<Is...>>>> {
        auto concat_two = [](auto &&t1, auto &&t2) {
            return std::apply([](auto &&... args) { return do_concat(std::forward<decltype(args)>(args)...); }, std::tuple_cat(std::move(t1), std::move(t2)));
        };
        auto tuples = std::make_tuple(std::move(first), std::move(last));
        auto result = std::apply([&](auto &&... args) { return (concat_two(std::forward<decltype(args)>(args)...)); }, tuples);
        return result;
    }

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
}

namespace genex::views {
    template <typename T1, typename T2>
    struct can_concat_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct can_concat_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (input_iterator<Is> and ...) and
            (sentinel_for<Ss, Is> and ...) and
            requires { typename std::common_type_t<iter_value_t<Is>...>; };
    };

    template <typename T1, typename T2>
    concept can_concat_iters = can_concat_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept can_concat_range =
        sizeof...(Rngs) > 0 and
        (input_range<Rngs> and ...) and
        requires { typename std::common_type_t<range_value_t<Rngs>...>; };

    DEFINE_VIEW(concat) {
        template <typename... Is, typename... Ss> requires can_concat_iters<std::tuple<Is...>, std::tuple<Ss...>>
        constexpr auto operator()(std::tuple<Is...> &&first, std::tuple<Ss...> &&last) const -> auto {
            auto gen = detail::do_concat(std::move(first), std::move(last));
            return concat_view(std::move(gen));
        }

        template <typename... Rngs> requires (can_concat_range<Rngs...> and sizeof...(Rngs) > 1)
        constexpr auto operator()(Rngs &&... ranges) const -> auto {
            auto gen = detail::do_concat(std::forward<Rngs>(ranges)...);
            return concat_view(std::move(gen));
        }

        template <typename Rng2> requires can_concat_range<Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires can_concat_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    EXPORT_GENEX_VIEW(concat);
}
