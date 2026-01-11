module;
#include <genex/macros.hpp>

export module genex.views2.duplicates;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
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


namespace genex::views::detail::impl {
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
                    if (meta::invoke(self.comp, cur_proj, meta::invoke(self.proj, *self.dupe_elem))) {
                        self.cur_elem = *self.it;
                        ++self.it;
                        return;
                    }
                    ++self.it;
                    continue;
                }

                // Search for the duplicate
                for (const auto &s: self.seen) {
                    if (meta::invoke(self.comp, cur_proj, meta::invoke(self.proj, s))) {
                        self.dupe_elem = s;
                        self.cur_elem = s;
                        self.pending = *self.it;
                        ++self.it;
                        return;
                    }
                }

                self.seen.push_back(*self.it);
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


namespace genex::views {
    struct duplicates_fn {
        template <typename I, typename S, typename Comp = operations::eq, typename Proj = meta::identity>
        requires detail::concepts::duplicate_checkable_iters<I, S, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
            return detail::impl::duplicate_view(std::move(first), std::move(last), std::move(comp), std::move(proj));
        }

        template <typename Rng, typename Comp = operations::eq, typename Proj = meta::identity>
        requires detail::concepts::duplicate_checkable_range<Rng, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::duplicate_view(std::move(first), std::move(last), std::move(comp), std::move(proj));
        }

        template <typename Comp = operations::eq, typename Proj = meta::identity>
        requires (not range<Comp>)
        GENEX_INLINE constexpr auto operator()(Comp comp = {}, Proj proj = {}) const {
            return meta::bind_back(duplicates_fn{}, std::move(comp), std::move(proj));
        }
    };

    export inline constexpr duplicates_fn duplicates{};
}
