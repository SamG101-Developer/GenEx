#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


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
        template <typename T = void>
        constexpr auto operator()(const std::size_t lo, const std::size_t hi, const std::size_t step = 1uz) const -> auto {
            auto gen = detail::do_iota<T>(lo, hi, step);
            return iota_view(std::move(gen));
        }

        template <typename T = void>
        constexpr auto operator()(const std::size_t hi) const -> auto {
            auto gen = detail::do_iota<T>(0uz, hi, 1uz);
            return iota_view(std::move(gen));
        }
    };

    EXPORT_GENEX_VIEW(iota);
}
