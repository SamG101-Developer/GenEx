module;
#include <functional>

export module genex.algorithms.all_of;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms {
    struct all_of_base_fn {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> bool {
            for (auto it = first; it != last; ++it) {
                if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *it))) {
                    return false;
                }
            }
            return true;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> bool {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    struct all_of_fn final : all_of_base_fn {
        using all_of_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr all_of_fn all_of;
}
