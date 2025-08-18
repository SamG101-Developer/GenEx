#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::actions::detail {
    template <range Rng, std::invocable<range_value_t<Rng>> Proj, std::invocable<std::invoke_result_t<Proj, range_value_t<Rng>>, std::invoke_result_t<Proj, range_value_t<Rng>>> F>
    auto do_sort(Rng *rng, F &&f, Proj &&proj) -> void {
        std::sort(iterators::begin(*rng), iterators::end(*rng), [f, proj]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
            return std::invoke(f, std::invoke(proj, std::forward<Lhs>(lhs)), std::invoke(proj, std::forward<Rhs>(rhs)));
        });
    }
}


namespace genex::actions {
    DEFINE_ACTION(sort) {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::invocable<std::invoke_result_t<Proj, range_value_t<Rng>>, std::invoke_result_t<Proj, range_value_t<Rng>>> F>
        constexpr auto operator()(Rng &&rng, F &&f, Proj &&proj = {}) const -> decltype(auto) {
            FWD_TO_IMPL(detail::do_sort, &rng, f, proj);
        }

        template <typename F, typename Proj = meta::identity>
        constexpr auto operator()(F &&f, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(f, proj);
        }
    };

    EXPORT_GENEX_ACTION(sort);
}
