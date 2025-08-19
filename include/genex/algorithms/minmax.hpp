#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <range Rng, std::invocable<range_value_t<Rng>> Proj, std::invocable<std::invoke_result_t<Proj, range_value_t<Rng>>, std::invoke_result_t<Proj, range_value_t<Rng>>> F>
    auto do_cmp(Rng &&rng, F &&f, Proj &&proj) -> range_value_t<Rng> {
        auto it = iterators::begin(rng);
        if (it == iterators::end(rng)) {
            throw std::runtime_error("Cannot find minimum/maximum of an empty range");
        }

        auto best_value = *it;
        auto best_proj_value = proj(best_value);

        for (++it; it != iterators::end(rng); ++it) {
            auto current_value = *it;
            auto current_proj_value = proj(current_value);
            if (f(current_proj_value, best_proj_value)) {
                best_value = current_value;
                best_proj_value = current_proj_value;
            }
        }

        return best_value;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(min) {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::do_cmp, rng, std::less<range_value_t<Rng>>{}, proj);
        }

        template <typename Proj = meta::identity>
        constexpr auto operator()(Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(proj);
        }
    };

    DEFINE_ALGORITHM(max) {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::do_cmp, rng, std::greater<range_value_t<Rng>>{}, proj);
        }

        template <typename Proj = meta::identity>
        constexpr auto operator()(Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(min);
    EXPORT_GENEX_ALGORITHM(max);
}
