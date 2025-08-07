module;
#include <coroutine>
#include <utility>
#include <genex/macros.hpp>

export module genex.views.slice;
export import genex.generator;
import genex.concepts;
import genex.iterators.advance;
import genex.iterators.begin;
import genex.iterators.distance;
import genex.iterators.end;
import genex.iterators.next;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S>
auto do_slice(I &&first, S &&last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) -> genex::generator<iter_value_t<I>> {
    for (; genex::iterators::next(first, start_index) != genex::iterators::next(last, end_index - 1); first = genex::iterators::next(first, step)) {
        co_yield *first;
    }
}


template <range Rng>
auto do_slice(Rng &&rng, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) -> genex::generator<range_value_t<Rng>> {
    for (auto it = genex::iterators::next(genex::iterators::begin(rng), start_index); it != genex::iterators::next(genex::iterators::begin(rng) + end_index - 1); genex::iterators::advance(it, step)) {
        co_yield *it;
    }
}


namespace genex::views {
    struct slice_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(do_slice, first, last, start_index, end_index, step);
        }

        template <range Rng>
        auto operator()(Rng &&rng, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(do_slice, rng, start_index, end_index, step);
        }

        auto operator()(const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> decltype(auto) {
            MAP_TO_BASE(start_index, end_index, step);
        }
    };

    EXPORT_GENEX_STRUCT(slice);
}
