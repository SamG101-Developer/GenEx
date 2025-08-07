module;
#include <coroutine>
#include <functional>
#include <genex/macros.hpp>

export module genex.views.reverse;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

template <iterator I, sentinel S>
auto do_reverse(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        co_yield *first;
    }
}

template <range Rng>
auto do_reverse(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
    for (auto it = genex::iterators::rbegin(rng); it != genex::iterators::rend(rng); ++it) {
        co_yield *it;
    }
}

namespace genex::views {
    struct reverse_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_reverse, first, last);
        }

        template <range Rng>
        auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_reverse, rng);
        }

        auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(reverse);
}
