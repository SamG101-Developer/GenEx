module;
#include <coroutine>
#include <functional>
#include <genex/macros.hpp>

export module genex.views.filter;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_filter(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> genex::generator<iter_value_t<I>> {
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
            co_yield *first;
        }
    }
}

template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_filter(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> genex::generator<range_value_t<Rng>> {
    for (auto &&x : rng) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) {
            co_yield std::forward<decltype(x)>(x);
        }
    }
}


namespace genex::views {
    struct filter_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_filter, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_filter, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };

    EXPORT_GENEX_STRUCT(filter);
}
