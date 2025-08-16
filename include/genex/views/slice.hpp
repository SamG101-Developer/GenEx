#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_slice(I &&first, S &&last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) -> generator<iter_value_t<I>> {
        for (; iterators::next(first, start_index) != iterators::next(last, end_index - 1); first = iterators::next(first, step)) {
            co_yield *first;
        }
    }


    template <range Rng>
    auto do_slice(Rng &&rng, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) -> generator<range_value_t<Rng>> {
        for (auto it = iterators::next(iterators::begin(rng), start_index); it != iterators::next(iterators::begin(rng) + end_index - 1); iterators::advance(it, step)) {
            co_yield *it;
        }
    }
}


namespace genex::views {
    struct slice_fn final : detail::view_base {
        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> generator<iter_value_t<I>> {
            MAP_TO_IMPL(detail::do_slice, first, last, start_index, end_index, step);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng, const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> generator<range_value_t<Rng>> {
            MAP_TO_IMPL(detail::do_slice, rng, start_index, end_index, step);
        }

        constexpr auto operator()(const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> decltype(auto) {
            MAP_TO_BASE(start_index, end_index, step);
        }
    };

    EXPORT_GENEX_STRUCT(slice);
}
