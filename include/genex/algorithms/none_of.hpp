#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_none_of(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> bool {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                return false;
            }
        }
        return true;
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_none_of(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> bool {
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) {
                return false;
            }
        }
        return true;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(none_of) {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::do_none_of, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::do_none_of, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            MAP_TO_BASE(pred, proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(none_of);
}
