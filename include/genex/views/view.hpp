#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S>
    auto do_view(I &&first, S &&last) -> generator<iter_value_t<I>> {
        for (auto it = first; it != last; ++it) {
            co_yield *it;
        }
    }

    template <range Rng>
    auto do_view(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::forward<decltype(x)>(x);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(view) {
        DEFINE_OUTPUT_TYPE(view);

        template <iterator I, sentinel_for<I> S>
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_view, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_view, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_VIEW(view);
}
