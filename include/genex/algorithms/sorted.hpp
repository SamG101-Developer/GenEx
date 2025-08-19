#pragma once
#include <functional>
#include <genex/actions/push.hpp>
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
    auto do_sorted(Rng &&rng, F &&f, Proj &&proj) -> std::remove_cvref_t<Rng> {
        auto copy = std::remove_cvref_t<Rng>();
        auto it = iterators::begin(rng);
        for (; it != iterators::end(rng); ++it) {
            copy |= actions::push_back(*it);
        }

        std::sort(iterators::begin(copy), iterators::end(copy), [f, proj]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
            return std::invoke(f, std::invoke(proj, std::forward<Lhs>(lhs)), std::invoke(proj, std::forward<Rhs>(rhs)));
        });

        return std::move(copy);
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(sorted) {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::invocable<std::invoke_result_t<Proj, range_value_t<Rng>>, std::invoke_result_t<Proj, range_value_t<Rng>>> F>
        constexpr auto operator()(Rng &&rng, F &&f, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::do_sorted, rng, f, proj);
        }

        template <typename F, typename Proj = meta::identity>
        constexpr auto operator()(F &&f, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(f, proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(sorted);
}
