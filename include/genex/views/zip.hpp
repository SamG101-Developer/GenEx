#pragma once
#include <coroutine>
#include <genex/categories.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <std::size_t I = 0, typename... Ts, typename... Us> requires (
        I <= sizeof...(Ts) and
        sizeof...(Ts) == sizeof...(Us))
    auto any_iterator_finished(std::tuple<Ts...> &starts, const std::tuple<Us...> &ends) -> bool {
        if constexpr (I < sizeof...(Ts)) {
            return std::get<I>(starts) == std::get<I>(ends) || any_iterator_finished<I + 1>(starts, ends);
        }
        else {
            return false;
        }
    }

    template <iterator... Is, sentinel_for<Is>... Ss> requires (
        sizeof...(Is) > 0 and
        sizeof...(Is) == sizeof...(Ss) and
        (categories::input_iterator<Is> and ...))
    auto do_zip(std::tuple<Is...> &&first, std::tuple<Ss...> &&last) -> generator<std::tuple<iter_value_t<Is>...>> {
        auto iters = std::move(first);
        auto ends = std::move(last);

        while (true) {
            if (any_iterator_finished(iters, ends)) { co_return; }
            co_yield std::apply([](auto &&... args) { return std::make_tuple(*args...); }, iters);
            std::apply([](auto &&... args) { ((++args), ...); }, iters);
        }
    }

    template <range... Rngs> requires (
        sizeof...(Rngs) > 0 and
        (categories::input_range<Rngs> and ...))
    auto do_zip(Rngs &&... ranges) -> generator<std::tuple<range_value_t<Rngs>...>> {
        auto iters = std::make_tuple(iterators::begin(ranges)...);
        auto ends = std::make_tuple(iterators::end(ranges)...);

        while (true) {
            if (any_iterator_finished(iters, ends)) { co_return; }
            co_yield std::apply([](auto &&... args) { return std::make_tuple(*args...); }, iters);
            std::apply([](auto &&... args) { ((++args), ...); }, iters);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(zip) {
        DEFINE_OUTPUT_TYPE(zip);

        template <iterator... Is, sentinel_for<Is>... Ss> requires (
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (categories::input_iterator<Is> and ...))
        constexpr auto operator()(std::tuple<Is...> &&first, std::tuple<Ss...> &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_zip, first, last);
        }

        template <range... Rngs> requires (
            sizeof...(Rngs) > 0 and
            (categories::input_range<Rngs> and ...))
        constexpr auto operator()(Rngs &&... ranges) const -> auto {
            auto gen = detail::do_zip(std::forward<Rngs>(ranges)...);
            GENERATOR_TO_VIEW;
        }
    };

    /**
     * @brief A view that zips multiple ranges or iterators together, yielding tuples of elements.
     *
     * This view takes multiple input ranges or tuples of iterators and combines them into a single range of tuples of
     * elements. The resulting tuples contain one element from each input range or iterator, effectively "zipping"
     * them together. Template constraints ensure that the length of all input ranges or iterator tuples are the same.
     */
    EXPORT_GENEX_VIEW(zip);
}
