module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.iota;
import genex.concepts;
import genex.generator;
import std;


namespace genex::views::detail {
    template <typename Int>
    requires std::weakly_incrementable<Int>
    auto do_iota(const Int lo, const Int hi, const Int step) -> generator<Int> {
        for (auto i = lo; i < hi; i += step) {
            co_yield i;
        }
    }
}


namespace genex::views {
    struct iota_fn {
        template <typename Int = std::size_t>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int lo, const Int hi, const Int step = static_cast<Int>(1)) const {
            return detail::do_iota<Int>(lo, hi, step);
        }

        template <typename Int = std::size_t>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int hi) const {
            return detail::do_iota<Int>(static_cast<Int>(0), hi, static_cast<Int>(1));
        }

        template <typename Int = std::size_t>
        requires std::weakly_incrementable<Int> and std::numeric_limits<Int>::is_specialized
        GENEX_INLINE constexpr auto operator()() const {
            return detail::do_iota<Int>(static_cast<Int>(0), std::numeric_limits<Int>::max(), static_cast<Int>(1));
        }
    };

    export inline constexpr iota_fn iota{};
}
