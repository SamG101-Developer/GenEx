#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S, typename E, std::invocable<E> Proj = genex::meta::identity>
auto do_count(I &&first, S &&last, E &&elem, Proj &&proj = {}) -> size_t {
    auto count = static_cast<std::size_t>(0);
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Proj>(proj), *first) == elem) { ++count; }
    }
    return count;
}


template <range Rng, typename E, std::invocable<E> Proj = genex::meta::identity>
auto do_count(Rng &&rng, E &&elem, Proj &&proj = {}) -> size_t {
    auto count = static_cast<std::size_t>(0);
    for (auto &&x : rng) {
        if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == elem) { ++count; }
    }
    return count;
}


namespace genex::algorithms {
    struct count_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> size_t {
            MAP_TO_IMPL(do_count, first, last, elem, proj);
        }

        template <range Rng, typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_count, rng, elem, proj);
        }

        template <typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(elem, proj);
        }
    };

    EXPORT_GENEX_STRUCT(count);
}
