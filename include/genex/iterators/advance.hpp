#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I>
    concept advancable_iters =
        std::input_iterator<I> and
        requires(I &it, iter_difference_t<I> n) { std::advance(it, n); };
}


namespace genex::iterators {
    struct advance_fn {
        template <typename I> requires concepts::advancable_iters<I>
        constexpr auto operator()(I &it, const iter_difference_t<I> n = 1) const -> I {
            std::advance(it, n);
            return it;
        }
    };

    EXPORT_GENEX_STRUCT(advance);
}
