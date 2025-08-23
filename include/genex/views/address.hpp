#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S> requires (
        categories::input_iterator<I> and
        requires { *std::declval<I>(); } and
        requires { **std::declval<I>(); })
    auto do_deref(I &&first, S &&last) -> generator<deref_value_t<iter_value_t<I>>> {
        for (auto it = first; it != last; ++it) {
            co_yield **it;
        }
    }

    template <range Rng> requires (
        categories::input_range<Rng> and
        requires { *std::declval<range_value_t<Rng>>(); })
    auto do_deref(Rng &&rng) -> generator<deref_value_t<range_value_t<Rng>>> {
        for (auto &&x : rng) {
            co_yield *x;
        }
    }

    template <iterator I, sentinel_for<I> S> requires (
        categories::input_iterator<I>)
    auto do_address(I &&first, S &&last) -> generator<iter_value_t<I>*> {
        for (auto it = first; it != last; ++it) {
            co_yield &*it;
        }
    }

    template <range Rng> requires (
        categories::input_range<Rng>)
    auto do_address(Rng &&rng) -> generator<range_value_t<Rng>*> {
        for (auto &&x : rng) {
            co_yield &x;
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(deref) {
        DEFINE_OUTPUT_TYPE(deref);

        template <iterator I, sentinel_for<I> S> requires (
            categories::input_iterator<I> and
            requires { *std::declval<I>(); } and
            requires { **std::declval<I>(); })
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_deref, first, last);
        }

        template <range Rng> requires (
            categories::input_range<Rng> and
            requires { *std::declval<range_value_t<Rng>>(); })
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_deref, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    DEFINE_VIEW(address) {
        DEFINE_OUTPUT_TYPE(deref);

        template <iterator I, sentinel_for<I> S> requires (
            categories::input_iterator<I>)
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_address, first, last);
        }

        template <range Rng> requires (
            categories::input_range<Rng>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_address, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_VIEW(deref);
    EXPORT_GENEX_VIEW(address);
}
