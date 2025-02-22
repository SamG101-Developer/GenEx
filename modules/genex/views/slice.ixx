module;
#include <iostream>
#include <coroutine>
#include <utility>

export module genex.views.slice;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.distance;
import genex.iterators.end;
import genex.iterators.next;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct slice_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> generator<range_value_t<Rng>> {
            for (auto&& x = iterators::next(iterators::begin(rng), start_index); x != iterators::next(iterators::begin(rng) + end_index - 1); x = iterators::next(x, step)) {
                co_yield *x;
            }
        }
    };

    struct slice_fn final : slice_base_fn {
        using slice_base_fn::operator();

        auto operator()(const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> decltype(auto) {
            return [this, start_index, end_index, step]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), start_index, end_index, step);
            };
        }
    };

    export inline constexpr slice_fn slice;
}
