module;
#include <functional>
#include <genex/macros.hpp>

export module genex.algorithms.count;
import genex.algorithms._algorithm_base;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;

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
        auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> size_t {
            MAP_TO_IMPL(do_count, first, last, elem, proj);
        }

        template <range Rng, typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_count, rng, elem, proj);
        }

        template <typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(elem, proj);
        }
    };

    EXPORT_GENEX_STRUCT(count);
}
