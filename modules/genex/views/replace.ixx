module;
#include <coroutine>
#include <functional>
#include <genex/macros.hpp>

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

// Todo: "replace_with" => callable function


template <iterator I, sentinel S>
auto do_replace(I &&first, S &&last, iter_value_t<I> &&old_val, iter_value_t<I> &&new_val) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        if (*first == old_val) { co_yield std::forward<decltype(new_val)>(new_val); }
        else { co_yield *first; }
    }
}


template <range Rng>
auto do_replace(Rng &&rng, range_value_t<Rng> &&old_val, range_value_t<Rng> &&new_val) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        if (x == old_val) { co_yield std::forward<decltype(new_val)>(new_val); }
        else { co_yield std::forward<decltype(x)>(x); }
    }
}


namespace genex::views {
    struct replace_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last, iter_value_t<I> &&old_val, iter_value_t<I> &&new_val) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_replace, first, last, old_val, new_val);
        }

        template <range Rng>
        auto operator()(Rng &&rng, range_value_t<Rng> &&old_val, range_value_t<Rng> &&new_val) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_replace, rng, old_val, new_val);
        }

        template <typename E>
        auto operator()(E &&old_val, E &&new_val) const -> decltype(auto) {
            MAP_TO_BASE(old_val, new_val);
        }
    };

    EXPORT_GENEX_STRUCT(replace);
}
