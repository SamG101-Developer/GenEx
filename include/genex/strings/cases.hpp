#pragma once
#include <coroutine>
#include <utility>
#include <cctype>
#include <cwctype>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/strings/_concepts.hpp>
#include <genex/pipe.hpp>


namespace genex::strings::concepts {
    template <typename I, typename S>
    concept can_case_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        char_like<iter_value_t<I>>;

    template <typename Rng>
    concept can_case_range =
        input_range<Rng> and
        can_case_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::strings::detail {
    template <typename I, typename S>
        requires (concepts::can_case_iters<I, S> and genex::strings::strict_char_like<iter_value_t<I>>)
    GENEX_NO_ASAN
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::toupper(*first));
        }
    }

    template <typename I, typename S>
        requires (concepts::can_case_iters<I, S> and genex::strings::wide_char_like<iter_value_t<I>>)
    GENEX_NO_ASAN
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::towupper(*first));
        }
    }

    template <typename I, typename S>
        requires (concepts::can_case_iters<I, S> and genex::strings::strict_char_like<iter_value_t<I>>)
    GENEX_NO_ASAN
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::tolower(*first));
        }
    }

    template <typename I, typename S>
        requires (concepts::can_case_iters<I, S> and genex::strings::wide_char_like<iter_value_t<I>>)
    GENEX_NO_ASAN
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::towlower(*first));
        }
    }
}


namespace genex::strings {
    struct upper_case_fn {
        template <typename I, typename S>
            requires concepts::can_case_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_upper_case(first, last);
        }

        template <typename Rng>
            requires concepts::can_case_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(upper_case_fn{});
        }
    };

    struct lower_case_fn {
        template <typename I, typename S>
            requires concepts::can_case_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_lower_case(first, last);
        }

        template <typename Rng>
            requires concepts::can_case_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(lower_case_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(upper_case);
    GENEX_EXPORT_STRUCT(lower_case);
}
