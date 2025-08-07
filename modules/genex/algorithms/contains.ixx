module;
#include <functional>
#include <genex/macros.hpp>

export module genex.algorithms.contains;
import genex.algorithms._algorithm_base;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;

using namespace genex::concepts;
using namespace genex::type_traits;


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


namespace genex::algorithms {
    struct contains_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> bool {
            MAP_TO_IMPL(do_contains, first, last, elem, proj);
        }

        template <range Rng, typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> bool {
            MAP_TO_IMPL(do_contains, rng, elem, proj);
        }

        template <typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(elem, proj);
        }
    };

    EXPORT_GENEX_STRUCT(contains);
}
