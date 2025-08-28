#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/iterators/_iterator_base.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
    template <typename I>
    concept can_advance_iters =
        std::input_iterator<I> and
        requires(I &it, iter_difference_t<I> n) { std::advance(it, n); };
}


namespace genex::iterators {
    DEFINE_ITERATOR(advance) {
        template <typename I> requires concepts::can_advance_iters<I>
        constexpr auto operator()(I &it, const iter_difference_t<I> n = 1) const noexcept(noexcept(std::advance(it, n))) -> I {
            std::advance(it, n);
            return it;
        }
    };

    EXPORT_GENEX_ITERATOR(advance);
}
