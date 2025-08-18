#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S> requires range<iter_value_t<I>>
    auto do_flat(I &&first, S &&last) -> generator<range_value_t<iter_value_t<I>>> {
        for (; first != last; ++first) {
            for (auto &&x : *first) {
                co_yield x;
            }
        }
    }

    template <range Rng> requires range<range_value_t<Rng>>
    auto do_flat(Rng &&rng) -> generator<range_value_t<range_value_t<Rng>>> {
        for (auto &&x : rng) {
            for (auto &&y : x) {
                co_yield std::forward<decltype(y)>(y);
            }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(flat) {
        DEFINE_OUTPUT_TYPE(flat);

        template <iterator I, sentinel S> requires range<iter_value_t<I>>
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_flat, first, last);
        }

        template <range Rng> requires range<range_value_t<Rng>>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_flat, rng);
        }

        constexpr auto operator()() const -> auto {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_VIEW(flat);
}
