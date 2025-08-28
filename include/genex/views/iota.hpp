#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename Size>
    concept can_iota_size = std::integral<Size>;
}


namespace genex::views::detail {
    template <typename Size> requires concepts::can_iota_size<Size>
    auto do_iota(const Size lo, const Size hi, const Size step) -> generator<Size> {
        for (auto i = lo; i < hi; i += step) {
            co_yield i;
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(iota) {
        template <typename Size = std::size_t> requires concepts::can_iota_size<Size>
        auto operator()(const Size lo, const Size hi, const Size step = static_cast<Size>(1)) const -> auto {
            auto gen = detail::do_iota<Size>(lo, hi, step);
            return iota_view(std::move(gen));
        }

        template <typename Size = std::size_t>
        auto operator()(const Size hi) const -> auto {
            auto gen = detail::do_iota<Size>(static_cast<Size>(0), hi, static_cast<Size>(1));
            return iota_view(std::move(gen));
        }
    };

    EXPORT_GENEX_VIEW(iota);
}
