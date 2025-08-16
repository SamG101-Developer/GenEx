#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/algorithms/count.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity>
    auto do_duplicates(I &&first, S &&last, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        auto orig = first;
        for (; first != last; ++first) {
            auto elem = *first;
            auto count_orig = orig;
            auto count = algorithms::count(std::move(count_orig), std::forward<I>(last), std::forward<decltype(elem)>(elem));

            if (count > 1) {
                for (; orig != last; ++orig) {
                    if (*orig == elem) { co_yield *orig; }
                }
                break;
            }
        }
    }


    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
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
    struct duplicates_fn final : detail::view_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity>
        constexpr auto operator()(I &&first, S &&last, Proj && proj = {}) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(detail::do_duplicates, first, last, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(detail::do_duplicates, rng, proj);
        }

        template <typename Proj = meta::identity>
        constexpr auto operator()(Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(proj);
        }
    };

    EXPORT_GENEX_STRUCT(duplicates);
}
