/**
* TODO: THIS DOESNT WORK
 */


module;
#include <coroutine>
#include <functional>

export module genex.views.copied;
import genex.concepts;
import genex.generator;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

namespace genex::views {
    struct copied_base_fn : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last) const -> generator<iter_value_t<I>> {
            for (auto it = first; it != last; ++it) {
                co_yield *it;
            }
        }

        template <range Rng>
        auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }
    };

    struct copied_fn final : copied_base_fn {
        using copied_base_fn::operator();

        auto operator()() const -> decltype(auto) {
            return [this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    export inline constexpr copied_fn copied;
}
