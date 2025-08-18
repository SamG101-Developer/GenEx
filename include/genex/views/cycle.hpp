#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_cycle(I &&first, S &&last) -> generator<iter_value_t<I>> {
        while (true) {
            for (auto it = first; it != last; ++it) { co_yield *it; }
        }
    }

    template <range Rng>
    auto do_cycle(Rng &&rng) -> generator<range_value_t<Rng>> {
        while (true) {
            for (auto &&x : rng) { co_yield std::forward<decltype(x)>(x); }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(cycle) {
        DEFINE_OUTPUT_TYPE(cycle);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_cycle, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_cycle, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_VIEW(cycle);
}
