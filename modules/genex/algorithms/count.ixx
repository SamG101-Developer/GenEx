module;
#include <functional>

export module genex.algorithms.count;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms {
    struct count_base_fn {
        template <iterator I, sentinel S, typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> size_t {
            auto count = size_t{0};
            for (auto it = first; it != last; ++it) {
                if (std::invoke(std::forward<Proj>(proj), *it) == elem) { ++count; }
            }
            return count;
        }

        template <range Rng, typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    struct count_fn final : count_base_fn {
        using count_base_fn::operator();

        template <typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            return [elem = std::forward<E>(elem), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr count_fn count;
}
