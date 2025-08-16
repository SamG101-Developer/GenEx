#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_enumerate(I &&first, S &&last) -> generator<std::pair<size_t, iter_value_t<I>>> {
        auto i = 0;
        for (; first != last; ++first) {
            co_yield std::make_pair(i++, *first);
        }
    }

    template <range Rng>
    auto do_enumerate(Rng &&rng) -> generator<std::pair<size_t, range_value_t<Rng>>> {
        auto i = 0;
        for (auto &&x : rng) {
            co_yield std::make_pair(i++, std::forward<decltype(x)>(x));
        }
    }
}


namespace genex::views {
    struct enumerate_fn final : detail::view_base {
        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const -> generator<std::pair<size_t, iter_value_t<I>>> {
            MAP_TO_IMPL(detail::do_enumerate, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> generator<std::pair<size_t, range_value_t<Rng>>> {
            MAP_TO_IMPL(detail::do_enumerate, rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(enumerate);
}
