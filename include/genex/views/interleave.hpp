#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I1, sentinel S1, iterator I2, iterator S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
    auto do_interleave(I1 &&first1, S1 &&last1, I2 first2, S2 last2) -> generator<iter_value_t<I1>> {
        while (first1 != last1 && first2 != last2) {
            co_yield *first1++;
            co_yield *first2++;
        }
        while (first1 != last1) { co_yield *first1++; }
        while (first2 != last2) { co_yield *first2++; }
    }


    template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
    auto do_interleave(Rng1 &&rng1, Rng2 &&rng2) -> generator<range_value_t<Rng1>> {
        auto it1 = iterators::begin(rng1);
        auto it2 = iterators::begin(rng2);
        auto end1 = iterators::end(rng1);
        auto end2 = iterators::end(rng2);

        while (it1 != end1 && it2 != end2) {
            co_yield *it1++;
            co_yield *it2++;
        }
        while (it1 != end1) { co_yield *it1++; }
        while (it2 != end2) { co_yield *it2++; }
    }
}


namespace genex::views {
    struct interleave_fn final : detail::view_base {
        template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
        constexpr auto operator()(I1 &&first1, S1 &&last1, I2 first2, S2 last2) const -> generator<iter_value_t<I1>> {
            MAP_TO_IMPL(detail::do_interleave, first1, last1, first2, last2);
        }

        template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> generator<range_value_t<Rng1>> {
            MAP_TO_IMPL(detail::do_interleave, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> decltype(auto) {
            MAP_TO_BASE(rng2);
        }
    };

    EXPORT_GENEX_STRUCT(interleave);
}
