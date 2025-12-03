module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.strings.cases;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.iterators.iter_pair;
import genex.meta;
import std;


namespace genex::strings::detail::concepts {
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


namespace genex::strings::detail::impl {
    template <typename I, typename S>
    requires (concepts::can_case_iters<I, S> and strict_char_like<iter_value_t<I>>)
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::toupper(*first));
        }
    }

    template <typename I, typename S>
    requires (concepts::can_case_iters<I, S> and wide_char_like<iter_value_t<I>>)
    auto do_upper_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::towupper(*first));
        }
    }

    template <typename I, typename S>
    requires (concepts::can_case_iters<I, S> and strict_char_like<iter_value_t<I>>)
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::tolower(*first));
        }
    }

    template <typename I, typename S>
    requires (concepts::can_case_iters<I, S> and wide_char_like<iter_value_t<I>>)
    auto do_lower_case(I first, S last) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            co_yield static_cast<iter_value_t<I>>(std::towlower(*first));
        }
    }
}


namespace genex::strings {
    struct upper_case_fn {
        template <typename I, typename S>
        requires detail::concepts::can_case_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_upper_case(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::can_case_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_upper_case(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(upper_case_fn{});
        }
    };

    struct lower_case_fn {
        template <typename I, typename S>
        requires detail::concepts::can_case_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_lower_case(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::can_case_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_lower_case(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(lower_case_fn{});
        }
    };

    export inline constexpr upper_case_fn upper_case{};
    export inline constexpr lower_case_fn lower_case{};
}
