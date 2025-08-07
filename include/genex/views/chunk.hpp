#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S>
auto do_chunk(I &&first, S &&last, size_t size) -> genex::generator<genex::generator<iter_value_t<I>>> {
    for (auto it = first; it != last;) {
        co_yield [it = std::move(it), last = std::forward<S>(last), size]() mutable -> genex::generator<iter_value_t<I>> {
            for (size_t i = 0; i < size && it != last; ++i, ++it) {
                co_yield *it;
            }
        }();
    }
}


template <range Rng>
auto do_chunk(Rng &&rng, size_t size) -> genex::generator<genex::generator<range_value_t<Rng>>> {
    for (auto it = genex::iterators::begin(rng); it != genex::iterators::end(rng);) {
        co_yield [it = it, size, rng]() mutable -> genex::generator<range_value_t<Rng>> {
            for (size_t i = 0; i < size && it != genex::iterators::end(rng); ++i, ++it) {
                co_yield *it;
            }
        }();
    }
}


namespace genex::views {
    struct chunk_fn final : detail::view_base {
        template <iterator I, sentinel S>
        auto operator()(I &&first, S &&last, size_t size) const -> generator<generator<iter_value_t<I>>> {
            MAP_TO_IMPL(do_chunk, first, last, size);
        }

        template <range Rng>
        auto operator()(Rng &&rng, size_t size) const -> generator<generator<range_value_t<Rng>>> {
            MAP_TO_IMPL(do_chunk, rng, size);
        }

        auto operator()(size_t n) const -> decltype(auto) {
            MAP_TO_BASE(n);
        }
    };

    EXPORT_GENEX_STRUCT(chunk);
}
