module;
#include <genex/macros.hpp>

export module genex.views2.filter;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj = meta::identity>
    concept filterable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj = meta::identity>
    concept filterable_range =
        input_range<Rng> and
        filterable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views2::detail::impl {
    struct filter_sentinel {};

    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::filterable_iters<I, S, Pred, Proj>
    struct filter_iterator {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Pred pred;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(filter_iterator)

        GENEX_INLINE constexpr filter_iterator() = default;

        GENEX_INLINE constexpr filter_iterator(I first, S last, Pred p, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            pred(std::move(p)), proj(std::move(proj)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            self.fwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            self.bwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.it;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(filter_sentinel) {
            return self.it == self.st;
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self&& self) -> void {
            while (self.it != self.st and not meta::invoke(self.pred, meta::invoke(self.proj, *self.it))) { ++self.it; }
        }

        template <typename Self> requires std::bidirectional_iterator<I>
        GENEX_INLINE constexpr auto bwd_to_valid(this Self&& self) -> void {
            while (self.it != self.st and not meta::invoke(self.pred, meta::invoke(self.proj, *self.it))) { --self.it; }
        }
    };

    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::filterable_iters<I, S, Pred, Proj>
    struct filter_view {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Pred pred;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        GENEX_INLINE constexpr filter_view(I first, S last, Pred p, Proj prj = {}) :
            it(std::move(first)), st(std::move(last)), pred(std::move(p)), proj(std::move(prj)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return filter_iterator{self.it, self.st, self.pred, self.proj};
        }

        template <typename Self>
        GENEX_ITER_END {
            return filter_sentinel{};
        }
    };
}


namespace genex::views2 {
    struct filter_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::filterable_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const {
            return detail::impl::filter_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::filterable_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::filter_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Pred, typename Proj = meta::identity>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
            return meta::bind_back(filter_fn{}, std::move(pred), std::move(proj));
        }
    };

    export inline constexpr filter_fn filter{};
}
