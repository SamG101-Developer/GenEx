#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>


namespace genex::views::detail {
    template <typename Int>
        requires integer_like<Int>
    auto do_iota(const Int lo, const Int hi, const Int step) -> generator<Int> {
        for (auto i = lo; i < hi; i += step) {
            co_yield i;
        }
    }
}


namespace genex::views {
    struct iota_fn {
        template <typename Int = std::size_t>
            requires integer_like<Int>
        constexpr auto operator()(const Int lo, const Int hi, const Int step = static_cast<Int>(1)) const -> auto {
            return detail::do_iota<Int>(lo, hi, step);
        }

        template <typename Int = std::size_t>
            requires integer_like<Int>
        constexpr auto operator()(const Int hi) const -> auto {
            return detail::do_iota<Int>(static_cast<Int>(0), hi, static_cast<Int>(1));
        }
    };

    EXPORT_GENEX_STRUCT(iota);
}
