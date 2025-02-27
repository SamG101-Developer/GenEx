module;
#include <coroutine>
#include <functional>

export module genex.views.filter;
export import genex.generator;
import genex.concepts;
import genex.meta;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct filter_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            for (auto &&x : rng) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) {
                    co_yield std::forward<decltype(x)>(x);
                }
            }
        }
    };

    struct filter_fn final : filter_base_fn {
        using filter_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr filter_fn filter;
}
