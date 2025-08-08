#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
auto do_concat(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) -> genex::generator<iter_value_t<I1>> {
    for (; first1 != last1; ++first1) { co_yield *first1; }
    for (; first2 != last2; ++first2) { co_yield *first2; }
}

template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
auto do_concat(Rng1 &&rng1, Rng2 &&rng2) -> genex::generator<range_value_t<Rng1>> {
    for (auto &&x : rng1) { co_yield std::forward<decltype(x)>(x); }
    for (auto &&x : rng2) { co_yield std::forward<decltype(x)>(x); }
}


namespace genex::views {
    struct concat_fn final : detail::view_base {
        template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
        constexpr auto operator()(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) const -> generator<iter_value_t<I1>> {
            MAP_TO_IMPL(do_concat, first1, last1, first2, last2);
        }

        template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> generator<range_value_t<Rng1>> {
            MAP_TO_IMPL(do_concat, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> decltype(auto) {
            MAP_TO_BASE(rng2);
        }
    };

    EXPORT_GENEX_STRUCT(concat);
}
