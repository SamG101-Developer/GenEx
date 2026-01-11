module;
#include <genex/macros.hpp>

export module genex.views2.take_while;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Pred, typename Proj>
    concept takeable_while_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept takeable_while_range =
        input_range<Rng> and
        takeable_while_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views::detail::impl {
    struct take_while_sentinel {};

    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::takeable_while_iters<I, S, Pred, Proj>
    struct take_while_iterator {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Pred pred;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(take_while_iterator)

        GENEX_INLINE constexpr take_while_iterator() = default;

        GENEX_INLINE constexpr take_while_iterator(I it, S st, Pred pred, Proj proj) :
            it(std::move(it)), st(std::move(st)),
            pred(std::move(pred)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            if (self.it != self.st and not meta::invoke(self.pred, meta::invoke(self.proj, *self.it))) {
                self.it = self.st;
            }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.it;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(take_while_sentinel) {
            return self.it == self.st;
        }
    };

    template <typename I, typename S, typename Pred, typename Proj>
    requires concepts::takeable_while_iters<I, S, Pred, Proj>
    struct take_while_view {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Pred pred;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        GENEX_INLINE constexpr take_while_view(I first, S last, Pred pred, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            pred(std::move(pred)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return take_while_iterator(self.it, self.st, self.pred, self.proj);
        }

        template <typename Self>
        GENEX_ITER_END {
            return take_while_iterator(self.st, self.st, self.pred, self.proj);
        }
    };
}


namespace genex::views {
    struct take_while_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::takeable_while_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const {
            return detail::impl::take_while_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::takeable_while_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::take_while_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Pred, typename Proj = meta::identity>
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
            return meta::bind_back(take_while_fn{}, std::move(pred), std::move(proj));
        }
    };

    export inline constexpr take_while_fn take_while{};
}
