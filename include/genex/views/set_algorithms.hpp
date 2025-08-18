#pragma once
#include <coroutine>
#include <genex/algorithms/contains.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
    auto do_set_difference(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (!algorithms::contains(first2, last2, *first1)) { co_yield *first1; }
        }
    }

    template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
    auto do_set_difference(Rng1 &&rng1, Rng2 &&rng2) -> generator<range_value_t<Rng1>> {
        for (auto &&x : rng1) {
            if (!algorithms::contains(rng2, x)) { co_yield std::forward<decltype(x)>(x); }
        }
    }

    template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
    auto do_set_intersection(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (algorithms::contains(first2, last2, *first1)) { co_yield *first1; }
        }
    }

    template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
    auto do_set_intersection(Rng1 &&rng1, Rng2 &&rng2) -> generator<range_value_t<Rng1>> {
        for (auto &&x : rng1) {
            if (algorithms::contains(rng2, x)) { co_yield std::forward<decltype(x)>(x); }
        }
    }

    template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
    auto do_set_symmetric_difference(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (!algorithms::contains(first2, last2, *first1)) { co_yield *first1; }
        }
        for (; first2 != last2; ++first2) {
            if (!algorithms::contains(first1, last1, *first2)) { co_yield *first2; }
        }
    }

    template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
    auto do_set_symmetric_difference(Rng1 &&rng1, Rng2 &&rng2) -> generator<range_value_t<Rng1>> {
        for (auto &&x : rng1) {
            if (!algorithms::contains(rng2, x)) { co_yield std::forward<decltype(x)>(x); }
        }
        for (auto &&x : rng2) {
            if (!algorithms::contains(rng1, x)) { co_yield std::forward<decltype(x)>(x); }
        }
    }

    template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
    auto do_set_union(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) { co_yield *first1; }
        for (; first2 != last2; ++first2) {
            if (!algorithms::contains(first1, last1, *first2)) { co_yield *first2; }
        }
    }

    template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
    auto do_set_union(Rng1 &&rng1, Rng2 &&rng2) -> generator<range_value_t<Rng1>> {
        for (auto &&x : rng1) { co_yield std::forward<decltype(x)>(x); }
        for (auto &&x : rng2) {
            if (!algorithms::contains(rng1, x)) { co_yield std::forward<decltype(x)>(x); }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(set_difference) {
        DEFINE_OUTPUT_TYPE(set_difference);

        template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
        constexpr auto operator()(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_difference, first1, last1, first2, last2);
        }

        template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_difference, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            MAKE_CLOSURE(rng2);
        }
    };

    DEFINE_VIEW(set_intersection) {
        DEFINE_OUTPUT_TYPE(set_intersection);

        template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
        constexpr auto operator()(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_intersection, first1, last1, first2, last2);
        }

        template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_intersection, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            MAKE_CLOSURE(rng2);
        }
    };

    DEFINE_VIEW(set_symmetric_difference) {
        DEFINE_OUTPUT_TYPE(set_symmetric_difference);

        template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
        constexpr auto operator()(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_symmetric_difference, first1, last1, first2, last2);
        }

        template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_symmetric_difference, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            MAKE_CLOSURE(rng2);
        }
    };

    DEFINE_VIEW(set_union) {
        DEFINE_OUTPUT_TYPE(set_union);

        template <iterator I1, sentinel S1, iterator I2, sentinel S2> requires (std::same_as<iter_value_t<I1>, iter_value_t<I2>>)
        constexpr auto operator()(I1 &&first1, S1 &&last1, I2 &&first2, S2 &&last2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_union, first1, last1, first2, last2);
        }

        template <range Rng1, range Rng2> requires (std::same_as<range_value_t<Rng1>, range_value_t<Rng2>>)
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_set_union, rng1, rng2);
        }

        template <range Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            MAKE_CLOSURE(rng2);
        }
    };

    EXPORT_GENEX_VIEW(set_difference);
    EXPORT_GENEX_VIEW(set_intersection);
    EXPORT_GENEX_VIEW(set_symmetric_difference);
    EXPORT_GENEX_VIEW(set_union);
}
