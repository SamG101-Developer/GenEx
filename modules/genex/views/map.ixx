module;
#include <coroutine>
#include <functional>

export module genex.views.map;
import genex.concepts;
import genex.generator;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct map_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>> F>
        auto operator()(Rng &&rng, F &&f) const -> generator<std::invoke_result_t<F, range_value_t<Rng>>> {
            for (auto &&x : rng) {
                co_yield std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x));
            }
        }
    };

    struct map_fn final : map_base_fn {
        using map_base_fn::operator();

        template <typename F>
        auto operator()(F &&f) const -> decltype(auto) {
            return [f = std::forward<F>(f), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(f));
            };
        }
    };

    export inline constexpr map_fn map;
}
