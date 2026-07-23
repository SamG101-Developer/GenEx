module;
#include <genex/macros.hpp>

export module genex.strings.cases;
export import genex.pipe;
import genex.views2.transform;
import genex.concepts;
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
    struct to_upper_fn {
        template <typename C>
        requires char_like<C>
        GENEX_INLINE constexpr auto operator()(C c) const -> C {
            if constexpr (wide_char_like<C>) { return static_cast<C>(std::towupper(c)); }
            else if constexpr (strict_char_like<C>) { return static_cast<C>(std::toupper(c)); }
            else { return c; } // no locale-independent case mapping for UTF code units
        }
    };

    struct to_lower_fn {
        template <typename C>
        requires char_like<C>
        GENEX_INLINE constexpr auto operator()(C c) const -> C {
            if constexpr (wide_char_like<C>) { return static_cast<C>(std::towlower(c)); }
            else if constexpr (strict_char_like<C>) { return static_cast<C>(std::tolower(c)); }
            else { return c; } // no locale-independent case mapping for UTF code units
        }
    };
}

namespace genex::strings {
    struct upper_case_fn {
        template <typename I, typename S>
        requires detail::concepts::can_case_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return views::transform(std::move(first), std::move(last), detail::impl::to_upper_fn{});
        }

        template <typename Rng>
        requires detail::concepts::can_case_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            return views::transform(std::forward<Rng>(rng), detail::impl::to_upper_fn{});
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(upper_case_fn{});
        }
    };

    struct lower_case_fn {
        template <typename I, typename S>
        requires detail::concepts::can_case_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return views::transform(std::move(first), std::move(last), detail::impl::to_lower_fn{});
        }

        template <typename Rng>
        requires detail::concepts::can_case_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            return views::transform(std::forward<Rng>(rng), detail::impl::to_lower_fn{});
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(lower_case_fn{});
        }
    };

    export inline constexpr upper_case_fn upper_case{};
    export inline constexpr lower_case_fn lower_case{};
}