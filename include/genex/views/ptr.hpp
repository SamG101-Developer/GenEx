#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S> requires (unique_ptr<deref_value_t<I>>)
    auto do_ptr_unique(I &&first, S &&last) -> generator<typename deref_value_t<I>::pointer> {
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <range Rng> requires (unique_ptr<range_value_t<Rng>>)
    auto do_ptr_unique(Rng &&rng) -> generator<typename range_value_t<Rng>::pointer> {
        for (auto &&x : rng) {
            co_yield x.get();
        }
    }
}


namespace genex::views {
    struct ptr_unique_fn final : detail::view_base {
        template <iterator I, sentinel S> requires (unique_ptr<deref_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last) const -> decltype(auto) {
            MAP_TO_IMPL(detail::do_ptr_unique, first, last);
        }

        template <range Rng> requires (unique_ptr<range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            MAP_TO_IMPL(detail::do_ptr_unique, rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(ptr_unique);
}
