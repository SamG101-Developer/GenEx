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
    auto do_move(I &&first, S &&last) -> generator<iter_value_t<I>&&> {
        for (; first != last; ++first) {
            co_yield std::move(*first);
        }
    }

    template <range Rng>
    auto do_move(Rng &&rng) -> generator<range_value_t<Rng>&&> {
        for (auto &&x : rng) {
            co_yield std::move(x);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(move) {
        DEFINE_OUTPUT_TYPE(move);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_move, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_move, rng);
        }

        template <typename F>
        constexpr auto operator()(F &&f) const -> auto {
            MAKE_CLOSURE(f);
        }
    };

    EXPORT_GENEX_VIEW(move);
}
