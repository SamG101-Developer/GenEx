#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel S, typename E, std::invocable<E> Proj>
    auto do_contains(I &&first, S &&last, E &&elem, Proj &&proj = {}) -> bool {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Proj>(proj), *first) == elem) { return true; }
        }
        return false;
    }

    template <range Rng, typename E, std::invocable<E> Proj>
    auto do_contains(Rng &&rng, E &&elem, Proj &&proj = {}) -> bool {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == elem) { return true; }
        }
        return false;
    }
}


namespace genex::algorithms {
    struct contains_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> bool {
            MAP_TO_IMPL(detail::do_contains, first, last, elem, proj);
        }

        template <range Rng, typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> bool {
            MAP_TO_IMPL(detail::do_contains, rng, elem, proj);
        }

        template <typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(elem, proj);
        }
    };

    EXPORT_GENEX_STRUCT(contains);
}
