module;
#include <coroutine>
#include <functional>

export module genex.views.iota;
import genex.generator;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;


namespace genex::views {
    struct iota_fn final : detail::view_base {
        auto operator()(const size_t hi) const -> generator<size_t> {
            return (*this)(0, hi);
        }

        auto operator()(const size_t lo, const size_t hi, const size_t step = 1) const -> generator<size_t> {
            for (auto i = lo; i < hi; i += step) { co_yield i; }
        }
    };

    export inline constexpr iota_fn iota;
}
