/**
 * TODO: THIS DOESNT WORK
 */


module;
#include <coroutine>
#include <functional>
#include <genex/macros.hpp>

export module genex.views.copied;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S>
auto do_copied(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
    for (auto it = first; it != last; ++it) {
        co_yield *it;
    }
}


template <range Rng>
auto do_copied(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        co_yield std::forward<decltype(x)>(x);
    }
}


namespace genex::views {
    struct copied_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_copied, first, last);
        }

        template <range Rng>
        auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_copied, rng);
        }

        auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(copied);
}
