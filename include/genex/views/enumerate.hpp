#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S> requires (
        categories::input_iterator<I>)
    auto do_enumerate(I &&first, S &&last) -> generator<std::pair<size_t, iter_value_t<I>>> {
        auto i = 0;
        for (; first != last; ++first) {
            co_yield std::make_pair(i++, *first);
        }
    }

    template <range Rng> requires (
        categories::input_range<Rng>)
    auto do_enumerate(Rng &&rng) -> generator<std::pair<size_t, range_value_t<Rng>>> {
        auto i = 0;
        for (auto &&x : rng) {
            co_yield std::make_pair(i++, std::forward<decltype(x)>(x));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(enumerate) {
        DEFINE_OUTPUT_TYPE(enumerate);

        template <iterator I, sentinel_for<I> S> requires (
            categories::input_iterator<I>)
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_enumerate, first, last);
        }

        template <range Rng> requires (
            categories::input_range<Rng>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_enumerate, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_VIEW(enumerate);
}
