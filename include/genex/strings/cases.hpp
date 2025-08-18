#pragma once
#include <coroutine>
#include <functional>
#include <cctype>
#include <cwctype>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/strings/_string_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::strings::detail {
    template <iterator I, sentinel S> requires (genex::strings::strict_char_like<iter_value_t<I>>)
    auto do_upper_case(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::toupper(*first);
        }
    }

    template <iterator I, sentinel S> requires (genex::strings::wide_char_like<iter_value_t<I>>)
    auto do_upper_case(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::towupper(*first);
        }
    }

    template <range Rng> requires (genex::strings::strict_char_like<range_value_t<Rng>>)
    auto do_upper_case(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::toupper(std::forward<decltype(x)>(x));
        }
    }

    template <range Rng> requires (genex::strings::wide_char_like<range_value_t<Rng>>)
    auto do_upper_case(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::towupper(std::forward<decltype(x)>(x));
        }
    }

    template <iterator I, sentinel S> requires (genex::strings::strict_char_like<iter_value_t<I>>)
    auto do_lower_case(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::tolower(*first);
        }
    }

    template <iterator I, sentinel S> requires (genex::strings::wide_char_like<iter_value_t<I>>)
    auto do_lower_case(I &&first, S &&last) -> genex::generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::towlower(*first);
        }
    }

    template <range Rng> requires (genex::strings::strict_char_like<range_value_t<Rng>>)
    auto do_lower_case(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::tolower(std::forward<decltype(x)>(x));
        }
    }

    template <range Rng> requires (genex::strings::wide_char_like<range_value_t<Rng>>)
    auto do_lower_case(Rng &&rng) -> genex::generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::towlower(std::forward<decltype(x)>(x));
        }
    }
}


namespace genex::strings {
    DEFINE_STRING(upper_case) {
        template <iterator I, sentinel S> requires (char_like<iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last) const -> generator<iter_value_t<I>> {
            FWD_TO_IMPL(detail::do_upper_case, first, last);
        }

        template <range Rng> requires (char_like<range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            FWD_TO_IMPL(detail::do_upper_case, rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    DEFINE_STRING(lower_case) {
        template <iterator I, sentinel S> requires (char_like<iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last) const -> generator<iter_value_t<I>> {
            FWD_TO_IMPL(detail::do_lower_case, first, last);
        }

        template <range Rng> requires (char_like<range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng) const -> generator<range_value_t<Rng>> {
            FWD_TO_IMPL(detail::do_lower_case, rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRING(upper_case);
    EXPORT_GENEX_STRING(lower_case);
}
