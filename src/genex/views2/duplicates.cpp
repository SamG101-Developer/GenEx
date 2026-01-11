module;
#include <genex/macros.hpp>

export module genex.views2.filter;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept duplicate_checkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_comparable<I, I, Comp, Proj, Proj>;

    template <typename Rng, typename Comp, typename Proj>
    concept duplicate_checkable_range =
        forward_range<Rng> and
        duplicate_checkable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::views2::detail::impl {
    struct duplicate_sentinel {};

    template <typename I, typename S, typename Comp, typename Proj>
    requires concepts::duplicate_checkable_iters<I, S, Comp, Proj>
    struct duplicate_iterator {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Comp comp;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        // Duplicate state attributes
        std::vector<iter_value_t<I>> seen;
        std::optional<iter_value_t<I>> dupe_elem;
        std::optional<iter_value_t<I>> pending;
        std::optional<iter_value_t<I>> cur_elem;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(duplicate_iterator)

        GENEX_INLINE constexpr duplicate_iterator() = default;

        GENEX_INLINE constexpr duplicate_iterator(I first, S last, Comp comp, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            comp(std::move(comp)), proj(std::move(proj)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            if (self.pending.has_value()) {
                self.cur_elem = std::move(self.pending);
                self.pending.reset();
                return self;
            }

            self.fwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV = delete;

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.cur_elem;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(duplicate_sentinel) {
            return self.it == self.st and not self.pending.has_value() and not self.cur_elem.has_value();
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self &&self) -> void {
            self.cur_elem.reset();

            while (self.it != self.st) {
                // Inspect the current element (via projection)
                decltype(auto) cur_proj = meta::invoke(self.proj, *self.it);

                if (self.dupe_elem.has_value()) {
                    if (meta::invoke(self.comp, cur_proj, meta::invoke(self.prok, *self.dupe_elem))) {
                        self.cur_elem = std::move(*self.it);
                        ++self.it;
                        return;
                    }
                    ++self.it;
                }

                // Search for the duplicate
                for (const auto &s: self.seen) {
                    if (meta::invoke(self.comp, cur_proj, meta::invoke(self.proj, s))) {
                        self.dupe_elem = s;
                        self.cur_elem = s;
                        self.pending = std::move(*self.it);
                        ++self.it;
                        return;
                    }
                }

                self.seen.push_back(std::move(*self.it));
                ++self.it;
            }
        }
    };

    template <typename I, typename S, typename Comp, typename Proj>
    requires concepts::duplicate_checkable_iters<I, S, Comp, Proj>
    struct duplicate_view {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Comp comp;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        GENEX_INLINE constexpr duplicate_view(I first, S last, Comp comp, Proj proj = {}) :
            it(std::move(first)), st(std::move(last)),
            comp(std::move(comp)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return duplicate_iterator(self.it, self.st, self.comp, self.proj);
        }

        template <typename Self>
        GENEX_ITER_END {
            return duplicate_sentinel();
        }
    };
}


namespace genex::views2 {
    struct duplicate_fn {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::duplicate_checkable_iters<I, S, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Pred pred, Proj proj = {}) const {
            return detail::impl::duplicate_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity>
        requires detail::concepts::duplicate_checkable_range<Rng, Pred, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::duplicate_view(std::move(first), std::move(last), std::move(pred), std::move(proj));
        }

        template <typename Pred, typename Proj = meta::identity>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred, Proj proj = {}) const {
            return meta::bind_back(duplicate_fn{}, std::move(pred), std::move(proj));
        }
    };

    export inline constexpr duplicate_fn duplicate{};
}
