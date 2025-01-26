module;
#include <coroutine>
#include <functional>

export module genex.views.reverse;
import genex.concepts;
import genex.generator;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

namespace genex::views {
    struct reverse_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            for (auto it = iterators::rbegin(rng); it != iterators::rend(rng); ++it) {
                co_yield *it;
            }
        }
    };

    struct reverse_fn final : reverse_base_fn {
        using reverse_base_fn::operator();

        auto operator()() const -> decltype(auto) {
            return [this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    export inline constexpr reverse_fn reverse;
}
