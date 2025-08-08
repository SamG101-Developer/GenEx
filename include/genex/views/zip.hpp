#pragma once
#include <coroutine>
#include <genex/concepts.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <std::size_t I = 0, typename... Ts>
auto any_iterator_finished(std::tuple<Ts...> &starts, const std::tuple<Ts...> &ends) -> bool {
    if constexpr (I < sizeof...(Ts)) {
        return (std::get<I>(starts) == std::get<I>(ends)) || any_iterator_finished<I + 1>(starts, ends);
    }
    else {
        return false;
    }
}


template <range... Rngs>
auto do_zip(Rngs &&... ranges) -> genex::generator<std::tuple<range_value_t<Rngs>...>> {
    auto iters = std::make_tuple(genex::iterators::begin(ranges)...);
    auto ends = std::make_tuple(genex::iterators::end(ranges)...);

    while (true) {
        if (any_iterator_finished(iters, ends)) { co_return; }
        co_yield std::apply([](auto &&... args) { return std::make_tuple(*args...); }, iters);
        std::apply([](auto &&... args) { ((++args), ...); }, iters);
    }
}


namespace genex::views {
    struct zip_fn final : detail::view_base {
        template <range... Rngs>
        constexpr auto operator()(Rngs &&... ranges) const -> generator<std::tuple<range_value_t<Rngs>...>> {
            return do_zip(std::forward<decltype(ranges)>(ranges)...);
        }

        constexpr auto operator()(const std::size_t start_index, const std::size_t end_index, const std::size_t step = 1) const -> decltype(auto) {
            MAP_TO_BASE(start_index, end_index, step);
        }
    };

    EXPORT_GENEX_STRUCT(zip);
}
