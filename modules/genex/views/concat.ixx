module;
#include <coroutine>
#include <functional>

export module genex.views.concat;
export import genex.generator;
import genex.concepts;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct concat_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng1, Rng &&rng2) const -> generator<range_value_t<Rng>> {
            for (auto &&x : rng1) { co_yield std::forward<decltype(x)>(x); }
            for (auto &&x : rng2) { co_yield std::forward<decltype(x)>(x); }
        }
    };

    struct concat_fn final : concat_base_fn {
        using concat_base_fn::operator();

        template <range Rng>
        auto operator()(Rng &&rng2) const -> decltype(auto) {
            return [rng2 = std::forward<Rng>(rng2), this](Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1), std::forward<Rng>(rng2));
            };
        }
    };

    export inline constexpr concat_fn concat;
}
