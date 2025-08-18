#pragma once
#include <coroutine>
#include <functional>
#include <vector>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj, std::invocable<std::invoke_result_t<Proj, iter_value_t<I>>, std::invoke_result_t<Proj, iter_value_t<I>>> F>
    auto do_sort(I &&first, S &&last, F &&f, const bool rev, Proj &&proj) -> generator<iter_value_t<I>> {
        auto tmp = std::vector<iter_value_t<I>>(iterators::distance(first, last));
        for (auto it = first; it != last; ++it) {
            tmp.emplace_back(*it);
        }

        std::sort(tmp.begin(), tmp.end(), [f, proj]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
            return std::invoke(f, std::invoke(proj, std::forward<Lhs>(lhs)), std::invoke(proj, std::forward<Rhs>(rhs)));
        });

        if (rev) {
            auto it = tmp.rbegin();
            auto end = tmp.rend();
            for (; it != end; ++it) { co_yield *it; }
        }
        else {
            auto it = tmp.begin();
            auto end = tmp.end();
            for (; it != end; ++it) { co_yield *it; }
        }
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj, std::invocable<std::invoke_result_t<Proj, range_value_t<Rng>>, std::invoke_result_t<Proj, range_value_t<Rng>>> F>
    auto do_sort(Rng &&rng, F &&f, bool rev, Proj &&proj) -> generator<range_value_t<Rng>> {
        return do_sort(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<F>(f), rev, std::forward<Proj>(proj));
    }
}


namespace genex::views {
    DEFINE_VIEW(sort) {
        DEFINE_OUTPUT_TYPE(sort);

        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::invocable<std::invoke_result_t<Proj, iter_value_t<I>>, std::invoke_result_t<Proj, iter_value_t<I>>> F>
        constexpr auto operator()(I &&first, S &&last, F &&f, bool rev = false, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_sort, first, last, f, rev, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::invocable<std::invoke_result_t<Proj, range_value_t<Rng>>, std::invoke_result_t<Proj, range_value_t<Rng>>> F>
        constexpr auto operator()(Rng &&rng, F &&f, bool rev = false, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_sort, rng, f, rev, proj);
        }

        template <typename F, typename Proj = meta::identity>
        constexpr auto operator()(F &&f, bool rev = false, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(f, rev, proj);
        }
    };

    EXPORT_GENEX_VIEW(sort);
}
