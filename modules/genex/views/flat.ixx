module;
#include <coroutine>
#include <functional>

export module genex.views.flat;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

namespace genex::views {
    struct flat_base_fn : detail::view_base {
        template <range Rng> requires range<range_value_t<Rng>>
        auto operator()(Rng &&rng) const -> generator<range_value_t<range_value_t<Rng>>> {
            for (auto &&x : rng) {
                for (auto &&y : x) {
                    co_yield y;
                }
            }
        }
    };

    struct flat_fn final : flat_base_fn {
        using flat_base_fn::operator();

        auto operator()() const -> decltype(auto) {
            return [this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    export inline constexpr flat_fn flat;
}
