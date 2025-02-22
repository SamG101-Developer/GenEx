module;
#include <coroutine>

export module genex.views.set_algorithms;
export import genex.generator;
import genex.algorithms.contains;
import genex.concepts;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

namespace genex::views {
    struct set_difference_base_fn : detail::view_base {
        template <range Rng1, range Rng2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> generator<range_value_t<Rng1>> {
            for (auto &&x : rng1) {
                if (!algorithms::contains(rng2, x)) { co_yield x; }
            }
        }
    };

    struct set_intersection_base_fn : detail::view_base {
        template <range Rng1, range Rng2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> generator<range_value_t<Rng1>> {
            for (auto &&x : rng1) {
                if (algorithms::contains(rng2, x)) { co_yield x; }
            }
        }
    };

    struct set_symmetric_difference_base_fn : detail::view_base {
        template <range Rng1, range Rng2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> generator<range_value_t<Rng1>> {
            for (auto &&x : rng1) {
                if (!algorithms::contains(rng2, x)) { co_yield x; }
            }
            for (auto &&x : rng2) {
                if (!algorithms::contains(rng1, x)) { co_yield x; }
            }
        }
    };

    struct set_union_base_fn : detail::view_base {
        template <range Rng1, range Rng2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> generator<range_value_t<Rng1>> {
            for (auto &&x : rng1) { co_yield x; }
            for (auto &&x : rng2) {
                if (!algorithms::contains(rng1, x)) { co_yield x; }
            }
        }
    };

    struct set_difference_fn final : set_difference_base_fn {
        using set_difference_base_fn::operator();

        template <range Rng>
        auto operator()(Rng &&rng2) const -> decltype(auto) {
            return [rng2 = std::forward<Rng>(rng2), this](Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1), std::forward<Rng>(rng2));
            };
        }
    };

    struct set_intersection_fn final : set_intersection_base_fn {
        using set_intersection_base_fn::operator();

        template <range Rng>
        auto operator()(Rng &&rng2) const -> decltype(auto) {
            return [rng2 = std::forward<Rng>(rng2), this](Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1), std::forward<Rng>(rng2));
            };
        }
    };

    struct set_symmetric_difference_fn final : set_symmetric_difference_base_fn {
        using set_symmetric_difference_base_fn::operator();

        template <range Rng>
        auto operator()(Rng &&rng2) const -> decltype(auto) {
            return [rng2 = std::forward<Rng>(rng2), this](Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1), std::forward<Rng>(rng2));
            };
        }
    };

    struct set_union_fn final : set_union_base_fn {
        using set_union_base_fn::operator();

        template <range Rng>
        auto operator()(Rng &&rng2) const -> decltype(auto) {
            return [rng2 = std::forward<Rng>(rng2), this](Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1), std::forward<Rng>(rng2));
            };
        }
    };


    export inline constexpr set_difference_fn set_difference;
    export inline constexpr set_intersection_fn set_intersection;
    export inline constexpr set_symmetric_difference_fn set_symmetric_difference;
    export inline constexpr set_union_fn set_union;
}
