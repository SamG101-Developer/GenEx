module;
#include <coroutine>
#include <functional>

export module genex.views.for_each;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

namespace genex::views {
    struct for_each_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>> F>
        auto operator()(Rng &&rng, F &&f) const -> void {
            for (auto &&x : rng) {
                std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x));
            }
        }
    };

    struct for_each_fn final : for_each_base_fn {
        using for_each_base_fn::operator();

        template <typename F>
        auto operator()(F &&f) const -> decltype(auto) {
            return [f = std::forward<F>(f), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(f));
            };
        }
    };

    export inline constexpr for_each_fn for_each;
}
