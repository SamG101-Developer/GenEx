#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_forward(I &&first, S &&last) -> generator<iter_value_t<I>&&> {
        for (; first != last; ++first) {
            co_yield std::forward<iter_value_t<I>>(*first);
        }
    }

    template <range Rng>
    auto do_forward(Rng &&rng) -> generator<range_value_t<Rng>&&> {
        for (auto &&x : rng) {
            co_yield std::forward<range_value_t<Rng>>(x);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(forward) {
        DEFINE_OUTPUT_TYPE(forward);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_forward, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_forward, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_VIEW(forward);
}
