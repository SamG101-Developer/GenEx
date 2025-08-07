module;
#include <cstdint>
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.iota;
export import genex.generator;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;


namespace genex::views {
    struct iota_fn final : detail::view_base {
        auto operator()(const std::size_t hi) const -> generator<std::size_t> {
            return (*this)(0, hi);
        }

        auto operator()(const std::size_t lo, const std::size_t hi, const std::size_t step = 1) const -> generator<std::size_t> {
            for (auto i = lo; i < hi; i += step) { co_yield i; }
        }
    };

    EXPORT_GENEX_STRUCT(iota);
}
