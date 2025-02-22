module;
#include <coroutine>
#include <functional>

export module genex.views.cycle;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct cycle_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng1) const -> generator<range_value_t<Rng>> {
            while (true) {
                for (auto &&x : rng1) { co_yield std::forward<decltype(x)>(x); }
            }
        }
    };

    struct cycle_fn final : cycle_base_fn {
        using cycle_base_fn::operator();
        auto operator()() const -> decltype(auto) {
            return [this]<range Rng>(Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1));
            };
        }
    };

    export inline constexpr cycle_fn cycle;
}

