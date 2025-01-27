module;
#include <coroutine>
#include <functional>

export module genex.views.intersperse;
import genex.concepts;
import genex.generator;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct interleave_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng1, Rng &&rng2) const -> generator<range_value_t<Rng>> {
            auto it1 = iterators::begin(rng1);
            auto it2 = iterators::begin(rng2);
            const auto end1 = iterators::end(rng1);
            const auto end2 = iterators::end(rng2);
            while (it1 != end1 && it2 != end2) {
                co_yield *it1++;
                co_yield *it2++;
            }
            while (it1 != end1) { co_yield *it1++; }
            while (it2 != end2) { co_yield *it2++; }
        }
    };

    struct interleave_fn final : interleave_base_fn {
        using interleave_base_fn::operator();

        template <range Rng>
        auto operator()(Rng &&rng2) const -> decltype(auto) {
            return [rng2 = std::forward<Rng>(rng2), this](Rng &&rng1) mutable {
                return (*this)(std::forward<Rng>(rng1), std::forward<Rng>(rng2));
            };
        }
    };

    export inline constexpr interleave_fn interleave;
}
