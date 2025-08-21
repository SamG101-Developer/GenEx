#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <typename T>
    auto do_iota(const std::size_t lo, const std::size_t hi, const std::size_t step) -> generator<std::size_t> {
        for (auto i = lo; i < hi; i += step) {
            co_yield i;
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(iota) {
        DEFINE_OUTPUT_TYPE(iota);

        template <typename T = void>
        constexpr auto operator()(const std::size_t lo, const std::size_t hi, const std::size_t step = 1uz) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_iota<T>, lo, hi, step);
        }

        template <typename T = void>
        constexpr auto operator()(const std::size_t hi) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_iota<T>, 0uz, hi, 1uz);
        }
    };

    EXPORT_GENEX_VIEW(iota);
}
