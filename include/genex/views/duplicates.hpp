#pragma once
#include <coroutine>
#include <genex/categories.hpp>
#include <genex/concepts.hpp>
#include <genex/algorithms/count.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S, typename Old2 = iter_value_t<I>, std::invocable<Old2> Proj = meta::identity> requires (
        categories::forward_iterator<I> and
        std::equality_comparable_with<iter_value_t<I>, Old2> and
        std::convertible_to<std::invoke_result_t<Proj, Old2>, iter_value_t<I>>)
    auto do_duplicates(I &&first, S &&last, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        auto orig = first;
        for (; first != last; ++first) {
            auto elem = *first;
            auto count_orig = orig;
            auto count = algorithms::count(std::move(count_orig), std::forward<I>(last), std::forward<decltype(elem)>(elem), std::forward<Proj>(proj));

            if (count > 1) {
                for (; orig != last; ++orig) {
                    if (*orig == elem) { co_yield *orig; }
                }
                break;
            }
        }
    }


    template <range Rng, typename Old2 = range_value_t<Rng>, std::invocable<Old2> Proj = meta::identity> requires (
        categories::forward_range<Rng> and
        std::equality_comparable_with<range_value_t<Rng>, Old2> and
        std::convertible_to<std::invoke_result_t<Proj, Old2>, range_value_t<Rng>>)
    auto do_duplicates(Rng &&rng, Proj &&proj = {}) -> generator<range_value_t<Rng>> {
        auto orig = iterators::begin(rng);
        for (auto &&x : rng) {
            auto count_orig = orig;
            auto count = algorithms::count(std::move(count_orig), iterators::end(rng), std::forward<decltype(x)>(x), std::forward<Proj>(proj));

            if (count > 1) {
                for (; orig != iterators::end(rng); ++orig) {
                    if (*orig == x) { co_yield std::forward<decltype(x)>(x); }
                }
                break;
            }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(duplicates) {
        DEFINE_OUTPUT_TYPE(duplicates);

        template <iterator I, sentinel_for<I> S, typename Old2 = iter_value_t<I>, std::invocable<Old2> Proj = meta::identity> requires (
            categories::forward_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old2> and
            std::convertible_to<std::invoke_result_t<Proj, Old2>, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_duplicates, first, last, proj);
        }

        template <range Rng, typename Old2 = range_value_t<Rng>, std::invocable<Old2> Proj = meta::identity> requires (
            categories::forward_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old2> and
            std::convertible_to<std::invoke_result_t<Proj, Old2>, range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_duplicates, rng, proj);
        }

        template <typename Proj = meta::identity>
        constexpr auto operator()(Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(proj);
        }
    };

    EXPORT_GENEX_VIEW(duplicates);
}
