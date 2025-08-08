#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


inline auto do_iota(const std::size_t lo, const std::size_t hi, const std::size_t step) -> genex::generator<std::size_t> {
    for (auto i = lo; i < hi; i += step) { co_yield i; }
}


namespace genex::views {
    struct iota_fn final : detail::view_base {
        constexpr auto operator()(const std::size_t lo, const std::size_t hi, const std::size_t step = 1) const -> generator<std::size_t> {
            MAP_TO_IMPL(do_iota, lo, hi, step);
        }

        constexpr auto operator()(const std::size_t hi) const -> generator<std::size_t> {
            MAP_TO_IMPL(do_iota, 0, hi, 1);
        }
    };

    EXPORT_GENEX_STRUCT(iota);
}
