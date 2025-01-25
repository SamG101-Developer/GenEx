module;
#include <functional>

export module genex.algorithms.binary_search;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms {
    struct binary_search_base_fn {
        template <iterator I, sentinel S, typename E, typename C = std::less<E>, std::invocable<iter_value_t<I>> Proj = meta::identity>
        auto operator()(I &&first, S &&last, E &&elem, C &&comp = {}, Proj &&proj = {}) const -> bool {
            first = std::lower_bound(std::forward<I>(first), std::forward<S>(last), std::forward<E>(elem), std::forward<C>(comp));
            return first != last && !(std::forward<E>(elem) < std::invoke(std::forward<Proj>(proj), *first));
        }

        template <range Rng, std::same_as<range_value_t<Rng>> E, typename C = std::less<E>, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        auto operator()(Rng &&rng, E &&elem, C &&comp = {}, Proj &&proj = {}) const -> bool {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<E>(elem), std::forward<C>(comp), std::forward<Proj>(proj));
        }
    };

    struct binary_search_fn final : binary_search_base_fn {
        using binary_search_base_fn::operator();

        template <typename E, typename C = std::less<E>, typename Proj = meta::identity>
        auto operator()(E &&elem, C &&comp = {}, Proj &&proj = {}) const -> decltype(auto) {
            return [elem = std::forward<E>(elem), comp = std::forward<C>(comp), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem), std::forward<C>(comp), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr binary_search_fn binary_search;
}
