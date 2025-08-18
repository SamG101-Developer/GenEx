#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/macros.hpp>

using namespace genex::concepts;


namespace genex::iterators {
    template <typename I>
    concept has_std_advance = requires(I &&it) { std::advance(std::forward<I>(it), 0); };

    template <typename I>
    concept has_operator_plusplus = requires(I &&it) { ++it; };

    template <typename I>
    concept has_advance = has_std_advance<I> || has_operator_plusplus<I>;

    DEFINE_ITERATOR(advance) {
        template <iterator I> requires (has_std_advance<I>)
        constexpr auto operator()(I &&it, const std::size_t n = 1) const noexcept -> I {
            std::advance(it, n);
            return it;
        }

        template <iterator I> requires (not has_std_advance<I> && has_operator_plusplus<I>)
        constexpr auto operator()(I &&it, const std::size_t n = 1) const noexcept -> I {
            for (std::size_t i = 0; i < n; ++i) { ++it; }
            return it;
        }
    };

    template <typename Rng>
    using advance_t = decltype(std::declval<advance_fn>()(std::declval<Rng>()));

    EXPORT_GENEX_ITERATOR(advance);
}
