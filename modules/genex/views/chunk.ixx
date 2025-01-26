module;
#include <coroutine>
#include <functional>

export module genex.views.chunk;
import genex.concepts;
import genex.generator;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

namespace genex::views {
    struct chunk_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, size_t size) const -> generator<generator<range_value_t<Rng>>> {
            for (auto it = iterators::begin(rng); it != iterators::end(rng);) {
                co_yield [it = it, size, rng, this]() mutable -> generator<range_value_t<Rng>> {
                    for (size_t i = 0; i < size && it != iterators::end(rng); ++i, ++it) {
                        co_yield *it;
                    }
                }();
            }
        }
    };

    struct chunk_fn final : chunk_base_fn {
        using chunk_base_fn::operator();

        auto operator()(size_t n) const -> decltype(auto) {
            return [this, n]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    export inline constexpr chunk_fn chunk;
}
