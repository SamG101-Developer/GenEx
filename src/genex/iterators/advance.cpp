module;
#include <genex/macros.hpp>

export module genex.iterators.advance;
import genex.concepts;
import std;


namespace genex::iterators::detail::concepts {
    template <typename I>
    concept advancable_iters =
        std::input_iterator<I> and
        requires(I &it, iter_difference_t<I> n) { std::advance(it, n); };
}


namespace genex::iterators {
    struct advance_fn {
        template <typename I>
        requires detail::concepts::advancable_iters<I>
        GENEX_INLINE constexpr auto operator()(I &it, const iter_difference_t<I> n = 1) const -> I {
            std::advance(it, n);
            return it;
        }
    };

    export inline constexpr advance_fn advance{};
}
