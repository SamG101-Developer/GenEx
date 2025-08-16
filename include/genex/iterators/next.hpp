#pragma once
#include <iterator>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/advance.hpp>

using namespace genex::concepts;


namespace genex::iterators {
    template <typename I>
    concept has_std_next = requires(I &&it) { std::next(std::forward<I>(it)); };

    template <typename I>
    concept has_next = has_std_next<I> || has_operator_plusplus<I>;

    struct next_fn final : detail::iterators_base {
        template <iterator I> requires (has_std_next<I>)
        constexpr auto operator()(I &&it, const std::size_t n = 1) const noexcept -> decltype(auto) {
            return std::next(std::forward<I>(it), n);
        }

        template <iterator I> requires (not has_std_next<I> && has_operator_plusplus<I>)
        constexpr auto operator()(I &&it, const std::size_t n = 1) const noexcept -> decltype(auto) {
            auto copy_it = std::forward<I>(it);
            for (std::size_t i = 0; i < n; ++i) { ++copy_it; }
            return copy_it;
        }
    };

    EXPORT_GENEX_STRUCT(next);
}
