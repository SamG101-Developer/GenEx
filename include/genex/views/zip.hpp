#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <std::size_t I = 0, typename... Ts, typename... Us> requires (I <= sizeof...(Ts) and sizeof...(Ts) == sizeof...(Us))
    auto any_iterator_finished(std::tuple<Ts...> &starts, const std::tuple<Us...> &ends) -> bool {
        if constexpr (I < sizeof...(Ts)) {
            return std::get<I>(starts) == std::get<I>(ends) || any_iterator_finished<I + 1>(starts, ends);
        }
        else {
            return false;
        }
    }

    template <typename Tuple, std::size_t... Is>
    auto deref_tuple_impl(Tuple &t, std::index_sequence<Is...>) {
        return std::tuple<iter_value_t<std::tuple_element_t<Is, Tuple>>...>{(*std::get<Is>(t))...};
    }

    template <typename Tuple>
    auto deref_tuple(Tuple &t) {
        return deref_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }

    template <typename Tuple, std::size_t... Is>
    void advance_tuple_impl(Tuple &t, std::index_sequence<Is...>) {
        ((++std::get<Is>(t)), ...);
    }

    template <typename Tuple>
    void advance_tuple(Tuple &t) {
        advance_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }

    template <typename... Is, typename... Ss>
    auto do_zip(std::tuple<Is...> first, std::tuple<Ss...> last) -> generator<std::tuple<iter_value_t<Is>...>> {
        auto begins = std::move(first);
        auto ends = std::move(last);

        while (!any_iterator_finished(begins, ends)) {
            co_yield deref_tuple(begins);
            advance_tuple(begins);
        }
    }
}


namespace genex::views {
    template <typename T1, typename T2>
    struct can_zip_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct can_zip_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (input_iterator<Is> and ...) and
            (sentinel_for<Ss, Is> and ...);
    };

    template <typename T1, typename T2>
    concept can_zip_iters = can_zip_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept can_zip_range =
        sizeof...(Rngs) > 0 and
        (input_range<Rngs> and ...);


    DEFINE_VIEW(zip) {
        template <typename... Is, typename... Ss> requires can_zip_iters<std::tuple<Is...>, std::tuple<Ss...>>
        constexpr auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const -> auto {
            auto gen = detail::do_zip(std::move(first), std::move(last));
            return zip_view(std::move(gen));
        }

        template <typename... Rngs> requires (can_zip_range<Rngs...> and sizeof...(Rngs) > 1)
        constexpr auto operator()(Rngs &&... ranges) const -> auto {
            return (*this)(std::make_tuple(iterators::begin(ranges)...), std::make_tuple(iterators::end(ranges)...));
        }

        template <typename Rng2> requires can_zip_range<Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires can_zip_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    EXPORT_GENEX_VIEW(zip);
}
