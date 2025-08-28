#pragma once
#include <coroutine>
#include <utility>
#include <cctype>
#include <cwctype>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/strings/_string_base.hpp>


namespace genex::strings::concepts {
    template <typename I, typename S>
    concept can_case_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        char_like<iter_value_t<I>>;

    template <typename Rng>
    concept can_case_range =
        input_range<Rng> and
        can_case_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::strings::detail {
    template <typename I, typename S> requires (concepts::can_case_iters<I, S> and genex::strings::strict_char_like<iter_value_t<I>>)
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::toupper(*first));
        }
    }

    template <typename I, typename S> requires (concepts::can_case_iters<I, S> and genex::strings::wide_char_like<iter_value_t<I>>)
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::towupper(*first));
        }
    }

    template <typename I, typename S> requires (concepts::can_case_iters<I, S> and genex::strings::strict_char_like<iter_value_t<I>>)
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::tolower(*first));
        }
    }

    template <typename I, typename S> requires (concepts::can_case_iters<I, S> and genex::strings::wide_char_like<iter_value_t<I>>)
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::towlower(*first));
        }
    }
}


namespace genex::strings {
    DEFINE_VIEW(upper_case) {
        template <typename I, typename S> requires concepts::can_case_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_upper_case(first, last);
            return upper_case_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_case_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_case_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(lower_case) {
        template <typename I, typename S> requires concepts::can_case_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_lower_case(first, last);
            return lower_case_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_case_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_case_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(upper_case);
    EXPORT_GENEX_VIEW(lower_case);
}
