module;
#include <genex/macros.hpp>

export module genex.views2.transform;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


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
    /**
     * The @c transform_iterator applies a transformation function to the elements of the underlying iterator. Note that
     * there is no special "sentinel" type for the @c transform_iterator, because the end is always known from the
     * underlying range.
     * @tparam I The type of the underlying iterator.
     * @tparam S The type of the underlying sentinel.
     * @tparam F The type of the transformation function.
     * @tparam Proj The type of the projection function.
     */
    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    struct transform_iterator {
        I it;
        GENEX_NO_UNIQUE_ADDRESS F f;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        using projected_reference_type = std::invoke_result_t<Proj&, iter_reference_t<I>>;
        using transformed_reference_type = std::invoke_result_t<F&, projected_reference_type>;

        using value_type = std::remove_cvref_t<transformed_reference_type>;
        using reference_type = transformed_reference_type;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(transform_iterator)

        GENEX_INLINE constexpr transform_iterator() = default;

        GENEX_INLINE constexpr transform_iterator(I it, F f, Proj proj = {}) :
            it(std::move(it)), f(std::move(f)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return meta::invoke(self.f, meta::invoke(self.proj, *self.it));
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(transform_iterator) {
            return self.it == that.it;
        }
    };

    template <typename I, typename S, typename F, typename Proj>
    requires concepts::transformable_iters<I, S, F, Proj>
    struct transform_view {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS F f;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        GENEX_INLINE constexpr transform_view(I first, S last, F f, Proj proj) :
            it(std::move(first)), st(std::move(last)), f(std::move(f)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return transform_iterator<I, S, F, Proj>{self.it, self.f, self.proj};
        }

        template <typename Self>
        GENEX_ITER_END {
            return transform_iterator<I, S, F, Proj>{self.st, self.f, self.proj};
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st);
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

    export inline constexpr transform_fn transform{};
}
