#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


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

    inline constexpr advance_fn advance{};
}
