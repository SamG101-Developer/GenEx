#pragma once
#include <coroutine>
#include <functional>
#include <cctype>
#include <cwctype>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/strings/_string_base.hpp>


namespace genex::strings::detail {
    template <typename I, typename S> requires (genex::strings::strict_char_like<iter_value_t<I>>)
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::toupper(*first);
        }
    }

    template <typename I, typename S> requires (genex::strings::wide_char_like<iter_value_t<I>>)
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::towupper(*first);
        }
    }

    template <typename Rng> requires (genex::strings::strict_char_like<range_value_t<Rng>>)
    auto do_upper_case(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::toupper(std::forward<decltype(x)>(x));
        }
    }

    template <typename Rng> requires (genex::strings::wide_char_like<range_value_t<Rng>>)
    auto do_upper_case(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::towupper(std::forward<decltype(x)>(x));
        }
    }

    template <typename I, typename S> requires (genex::strings::strict_char_like<iter_value_t<I>>)
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::tolower(*first);
        }
    }

    template <typename I, typename S> requires (genex::strings::wide_char_like<iter_value_t<I>>)
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield std::towlower(*first);
        }
    }

    template <typename Rng> requires (genex::strings::strict_char_like<range_value_t<Rng>>)
    auto do_lower_case(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::tolower(std::forward<decltype(x)>(x));
        }
    }

    template <typename Rng> requires (genex::strings::wide_char_like<range_value_t<Rng>>)
    auto do_lower_case(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::towlower(std::forward<decltype(x)>(x));
        }
    }
}


namespace genex::strings {
    template <typename I, typename S>
    concept can_case_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        char_like<iter_value_t<I>>;

    template <typename Rng>
    concept can_case_range =
        input_range<Rng> and
        char_like<range_value_t<Rng>>;

    DEFINE_VIEW(upper_case) {
        template <typename I, typename S> requires can_case_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_upper_case(first, last);
            return upper_case_view(std::move(gen));
        }

        template <typename Rng> requires can_case_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto gen = detail::do_upper_case(std::forward<Rng>(rng));
            return upper_case_view(std::move(gen));
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_case_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(lower_case) {
        template <typename I, typename S> requires can_case_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_lower_case(first, last);
            return lower_case_view(std::move(gen));
        }

        template <typename Rng> requires can_case_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto gen = detail::do_lower_case(std::forward<Rng>(rng));
            return lower_case_view(std::move(gen));
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_case_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(upper_case);
    EXPORT_GENEX_VIEW(lower_case);
}
