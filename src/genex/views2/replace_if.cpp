module;
#include <genex/macros.hpp>

export module genex.views2.replace_if;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Pred, typename New, typename Proj>
    concept replaceable_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::convertible_to<New, iter_value_t<I>>;

    template <typename Rng, typename Pred, typename New, typename Proj>
    concept replaceable_if_range =
        input_range<Rng> and
        replaceable_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, New, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    struct replace_iterator {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Pred pred;
        GENEX_NO_UNIQUE_ADDRESS New new_value;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(replace_iterator)

        GENEX_INLINE constexpr replace_iterator() = default;

        GENEX_INLINE constexpr replace_iterator(I first, S last, Pred pred, New new_val, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            pred(std::move(pred)), new_value(std::move(new_val)),
            proj(std::move(proj)) {
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
            return meta::invoke(self.pred, meta::invoke(self.proj, *self.it)) ? *&self.new_value : *self.it;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(replace_iterator) {
            return self.it == that.it;
        }
    };


    template <typename I, typename S, typename Pred, typename New, typename Proj>
    requires concepts::replaceable_if_iters<I, S, Pred, New, Proj>
    struct replace_view {
        I it;
        S st;
        GENEX_NO_UNIQUE_ADDRESS Pred pred;
        GENEX_NO_UNIQUE_ADDRESS New new_value;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        GENEX_INLINE constexpr replace_view(I first, S last, Pred pred, New new_val, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            pred(std::move(pred)), new_value(std::move(new_val)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return replace_iterator(self.it, self.st, self.pred, self.new_value, self.proj);
        }

        template <typename Self>
        GENEX_ITER_END {
            return replace_iterator(self.st, self.st, self.pred, self.new_value, self.proj);
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st);
        }
    };
}


namespace genex::views {
    struct replace_if_fn {
        template <typename I, typename S, typename Pred, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_if_iters<I, S, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, New new_value, Proj proj = {}) const {
            return detail::replace_view(std::move(first), std::move(last), std::move(pred), std::move(new_value), std::move(proj));
        }

        template <typename Rng, typename Pred, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, New new_value, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::replace_view(std::move(first), std::move(last), std::move(pred), std::move(new_value), std::move(proj));
        }

        template <typename Pred, typename New, typename Proj = meta::identity>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred, New new_value, Proj proj = {}) const {
            return meta::bind_back(replace_if_fn{}, std::move(pred), std::move(new_value), std::move(proj));
        }
    };

    export inline constexpr replace_if_fn replace_if{};
}
