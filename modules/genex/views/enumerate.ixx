module;
#include <coroutine>
#include <functional>

export module genex.views.enumerate;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct enumerate_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng1) const -> generator<std::pair<size_t, range_value_t<Rng>>> {
            auto i = 0;
            for (auto &&x : rng1) {
                co_yield std::make_pair(i++, std::forward<decltype(x)>(x));
            }
        }
    };

    struct enumerate_fn final : enumerate_base_fn {
        using enumerate_base_fn::operator();

        auto operator()() const -> decltype(auto) {
            return [this]<range Rng>(Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1));
            };
        }
    };

    export inline constexpr enumerate_fn enumerate;
}
