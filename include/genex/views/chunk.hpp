#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S> requires (
        categories::forward_iterator<I>)
    auto do_chunk(I &&first, S &&last, size_t size) -> generator<generator<iter_value_t<I>>> {
        for (auto it = first; it != last;) {
            co_yield [it = std::move(it), last = std::forward<S>(last), size]() mutable -> generator<iter_value_t<I>> {
                for (size_t i = 0; i < size && it != last; ++i, ++it) {
                    co_yield *it;
                }
            }();
        }
    }

    template <range Rng> requires (
        categories::forward_range<Rng>)
    auto do_chunk(Rng &&rng, size_t size) -> generator<generator<range_value_t<Rng>>> {
        for (auto it = iterators::begin(rng); it != iterators::end(rng);) {
            co_yield [it = it, size, rng]() mutable -> generator<range_value_t<Rng>> {
                for (size_t i = 0; i < size && it != iterators::end(rng); ++i, ++it) {
                    co_yield *it;
                }
            }();
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(chunk) {
        DEFINE_OUTPUT_TYPE(chunk);

        template <iterator I, sentinel_for<I> S> requires (
            categories::forward_iterator<I>)
        constexpr auto operator()(I &&first, S &&last, size_t size) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_chunk, first, last, size);
        }

        template <range Rng> requires (
            categories::forward_range<Rng>)
        constexpr auto operator()(Rng &&rng, size_t size) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_chunk, rng, size);
        }

        constexpr auto operator()(size_t n) const -> auto {
            MAKE_CLOSURE(n);
        }
    };

    EXPORT_GENEX_VIEW(chunk);
}
