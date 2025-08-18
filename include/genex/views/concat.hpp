#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I1, iterator I2, sentinel S1, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
    auto do_concat(I1 &&first1, I2 &&first2, S1 &&last1, S2 &&last2) -> generator<iter_value_t<I1>> {
        for (auto it = first1; it != last1; ++it) { co_yield *it; }
        for (auto it = first2; it != last2; ++it) { co_yield *it; }
    }

    template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
    auto do_concat(Rng1 &&rng1, Rng2 &&rng2) -> generator<range_value_t<Rng1>> {
        for (auto &&x : rng1) { co_yield std::forward<decltype(x)>(x); }
        for (auto &&x : rng2) { co_yield std::forward<decltype(x)>(x); }
    }
}

namespace genex::views {
    DEFINE_VIEW(concat) {
        DEFINE_OUTPUT_TYPE(concat);

        template <iterator I1, iterator I2, sentinel S1, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
        constexpr auto operator()(I1 &&i1, I2 &&i2, S1 &&s1, S2 &&s2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_concat, i1, i2, s1, s2);
        }

        template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_concat, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            MAKE_CLOSURE(rng2);
        }
    };

    EXPORT_GENEX_VIEW(concat);
}
