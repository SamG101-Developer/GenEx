#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S, std::invocable<iter_value_t<I>> F>
    auto do_for_each(I &&first, S &&last, F &&f) -> void {
        for (; first != last; ++first) {
            std::invoke(std::forward<F>(f), std::forward<decltype(*first)>(*first));
        }
    }


    template <range Rng, std::invocable<range_value_t<Rng>> F>
    auto do_for_each(Rng &&rng, F &&f) -> void {
        for (auto &&x : rng) {
            std::invoke(std::forward<F>(f), std::forward<decltype(x)>(x));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(for_each) {
        DEFINE_OUTPUT_TYPE(for_each);

        template <iterator I, sentinel_for<I> S, std::invocable<iter_value_t<I>> F>
        constexpr auto operator()(I &&first, S &&last, F &&f) const -> void {
            FWD_TO_IMPL_VIEW_VOID(detail::do_for_each, first, last, f);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> F>
        constexpr auto operator()(Rng &&rng, F &&f) const -> void {
            FWD_TO_IMPL_VIEW_VOID(detail::do_for_each, rng, f);
        }

        template <typename F>
        constexpr auto operator()(F &&f) const -> auto {
            MAKE_CLOSURE(f);
        }
    };

    EXPORT_GENEX_VIEW(for_each);
}
