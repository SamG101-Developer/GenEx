module;
#include <coroutine>
#include <functional>

export module genex.views.replace;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct replace_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, range_value_t<Rng> const &old_val, range_value_t<Rng> const &new_val) const -> generator<range_value_t<Rng>> {
            for (auto &&x : rng) {
                if (x == old_val) { co_yield new_val; }
                else { co_yield std::forward<decltype(x)>(x); }
            }
        }
    };

    struct replace_fn final : replace_base_fn {
        using replace_base_fn::operator();

        template <typename E>
        auto operator()(E &&old_val, E &&new_val) const -> decltype(auto) {
            return [old_val = std::forward<E>(old_val), new_val = std::forward<E>(new_val), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(old_val), std::forward<E>(new_val));
            };
        }
    };

    export inline constexpr replace_fn replace;
}
