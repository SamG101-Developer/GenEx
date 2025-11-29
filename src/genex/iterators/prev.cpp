module;
#include <genex/macros.hpp>

export module genex.iterators.prev;
import std;


namespace genex::iterators::detail::concepts {
    template <typename I>
    concept prevable_iters =
        std::bidirectional_iterator<I>;
}


namespace genex::iterators {
    struct prev_fn {
        template <typename I>
        requires detail::concepts::prevable_iters<I> and std::copyable<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n = 1) const -> I {
            return std::prev(std::move(it), n);
        }

        template <typename I>
        requires detail::concepts::prevable_iters<I> and std::copyable<I>
        GENEX_INLINE auto operator()(I it, const std::ptrdiff_t n, const I) const -> I {
            auto res = std::prev(std::move(it), n); // todo: use bound
            return res;
        }
    };

    export inline constexpr prev_fn prev{};
}
t