module;
#include <iterator>

export module genex.iterators.next;
import genex.iterators.advance;
import genex.iterators.begin;
import genex.concepts;
import genex.type_traits;


namespace genex::iterators {
    template <typename I>
    concept has_std_next = requires(I &&it) { std::next(std::forward<I>(it)); };

    export template <typename I>
    concept has_next = has_std_next<I> || has_operator_plusplus<I>;

    struct next_fn {
        template <concepts::iterator I> requires has_std_next<I>
        constexpr auto operator()(I it, const std::size_t n = 1) const noexcept -> decltype(auto) {
            return std::next(it, n);
        }

        template <concepts::iterator I> requires not has_std_next<I> && has_operator_plusplus<I>
        constexpr auto operator()(I it, const std::size_t n = 1) const noexcept -> decltype(auto) {
            auto copy_it = it;
            for (std::size_t i = 0; i < n; ++i) { ++copy_it; }
            return copy_it;
        }
    };

    export inline constexpr next_fn next;
}
