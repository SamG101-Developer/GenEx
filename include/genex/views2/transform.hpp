#pragma once
#include <genex/views2/view_base.hpp>


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename F, typename Proj = meta::identity>
    concept transformable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_unary_invocable<Proj, I> and
        std::indirectly_unary_invocable<F, std::projected<I, Proj>>;

    template <typename Rng, typename F, typename Proj = meta::identity>
    concept transformable_range =
        input_range<Rng> and
        transformable_iters<iterator_t<Rng>, sentinel_t<Rng>, F, Proj>;
}


namespace genex::views2::detail::impl {
    struct transform_sentinel {};

    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    struct transform_iterator : view_iterator_base {
        I it; S st;
        [[no_unique_address]] F f;
        [[no_unique_address]] Proj proj;

        using projected_reference_type = std::invoke_result_t<Proj&, iter_reference_t<I>>;
        using transformed_reference_type = std::invoke_result_t<F&, projected_reference_type>;

        using value_type = std::remove_cvref_t<transformed_reference_type>;
        using reference_type = transformed_reference_type;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_FORWARD_ITERATOR_OPERATIONS;

        GENEX_INLINE constexpr transform_iterator() = default;

        GENEX_INLINE constexpr transform_iterator(I first, S last, F f, Proj proj = {}) :
            it(std::move(first)), st(std::move(last)), f(std::move(f)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return meta::invoke(self.f, meta::invoke(self.proj, *self.it));
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_EQ_SENTINEL(transform_sentinel) {
            return self.it == self.st;
        }
    };

    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    struct transform_view : view_base {
        I it; S st;
        [[no_unique_address]] F f;
        [[no_unique_address]] Proj proj;

        GENEX_INLINE constexpr transform_view(I first, S last, F f, Proj proj = {}) :
            it(std::move(first)), st(std::move(last)), f(std::move(f)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_ITER_CUSTOM_BEGIN {
            return transform_iterator{self.it, self.st, self.f, self.proj};
        }

        template <typename Self>
        GENEX_ITER_CUSTOM_END {
            return transform_sentinel{};
        }
    };
}


namespace genex::views2 {
    struct transform_fn {
        template <typename I, typename S, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_iters<I, S, F, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, F f, Proj proj = {}) const {
            return detail::impl::transform_view(std::move(first), std::move(last), std::move(f), std::move(proj));
        }

        template <typename Rng, typename F, typename Proj = meta::identity>
        requires detail::concepts::transformable_range<Rng, F, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, F f, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::transform_view(std::move(first), std::move(last), std::move(f), std::move(proj));
        }

        template <typename F, typename Proj = meta::identity>
        requires (not range<F>)
        GENEX_INLINE constexpr auto operator()(F f, Proj proj = {}) const {
            return meta::bind_back(transform_fn{}, std::move(f), std::move(proj));
        }
    };

    inline constexpr transform_fn transform{};
}
