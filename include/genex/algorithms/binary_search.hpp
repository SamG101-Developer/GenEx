#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel S, typename E, typename C = std::less<E>, std::invocable<iter_value_t<I>> Proj = meta::identity>
    auto do_binary_search(I &&first, S &&last, E &&elem, C &&comp = {}, Proj &&proj = {}) -> bool {
        first = std::lower_bound(std::forward<I>(first), std::forward<S>(last), std::forward<E>(elem));
        return first != last && !std::invoke(std::forward<C>(comp), std::invoke(std::forward<Proj>(proj), *first), std::forward<E>(elem));
    }

    template <range Rng, std::same_as<range_value_t<Rng>> E, typename C = std::less<E>, std::invocable<range_value_t<Rng>> Proj = meta::identity>
    auto do_binary_search(Rng &&rng, E &&elem, C &&comp = {}, Proj &&proj = {}) -> bool {
        return do_binary_search(iterators::begin(rng), iterators::end(rng), std::forward<E>(elem), std::forward<C>(comp), std::forward<Proj>(proj));
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(binary_search) {
        template <iterator I, sentinel S, typename E, typename C = std::less<E>, std::invocable<iter_value_t<I>> Proj = meta::identity>
        constexpr auto operator()(I &&first, S &&last, E &&elem, C &&comp = {}, Proj &&proj = {}) const -> bool {
            FWD_TO_IMPL(detail::do_binary_search, first, last, elem, comp, proj);
        }

        template <range Rng, std::same_as<range_value_t<Rng>> E, typename C = std::less<E>, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, E &&elem, C &&comp = {}, Proj &&proj = {}) const -> bool {
            FWD_TO_IMPL(detail::do_binary_search, rng, elem, comp, proj);
        }

        template <typename E, typename C = std::less<E>, typename Proj = meta::identity>
        constexpr auto operator()(E &&elem, C &&comp = {}, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(elem, comp, proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(binary_search);
}
