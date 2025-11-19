#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename F>
    concept left_foldable_first_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>> and
        std::invocable<F, iter_value_t<I>, iter_reference_t<I>> and
        std::convertible_to<std::invoke_result_t<F, iter_value_t<I>, iter_reference_t<I>>, iter_value_t<I>>;

    template <typename Rng, typename F>
    concept left_foldable_first_range =
        forward_range<Rng> and
        left_foldable_first_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename F>
    requires concepts::left_foldable_first_iters<I, S, F>
    GENEX_INLINE constexpr auto do_fold_left_first(I first, S last, F &&f) -> iter_value_t<I> {
        auto acc = *first;
        ++first;
        for (; first != last; ++first) {
            acc = meta::invoke(f, std::move(acc), *first);
        }
        return acc;
    }
}


namespace genex {
    struct fold_left_first_fn {
        template <typename I, typename S, typename F>
        requires algorithms::detail::concepts::left_foldable_first_iters<I, S, F>
        GENEX_INLINE constexpr auto operator()(I first, S last, F &&f) const {
            return algorithms::detail::impl::do_fold_left_first(std::move(first), std::move(last), std::forward<F>(f));
        }

        template <typename Rng, typename F>
        requires algorithms::detail::concepts::left_foldable_first_range<Rng, F>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F &&f) const {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_fold_left_first(std::move(first), std::move(last), std::forward<F>(f));
        }
    };

    inline constexpr fold_left_first_fn fold_left_first{};
}
