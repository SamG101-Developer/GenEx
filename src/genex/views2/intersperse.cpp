module;
#include <genex/macros.hpp>

export module genex.views2.intersperse;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename New>
    concept interspersable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::constructible_from<New, iter_value_t<I>> and
        std::common_with<iter_value_t<I>, New> and
        std::copy_constructible<New> and
        std::copy_constructible<I>;

    template <typename Rng, typename New>
    concept interspersable_range =
        forward_range<Rng> and
        interspersable_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail::impl {
    struct intersperse_sentinel {};

    template <typename I, typename S, typename New>
    requires concepts::interspersable_iters<I, S, New>
    struct intersperse_iterator {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS I base_it;
        GENEX_NO_UNIQUE_ADDRESS New new_value;
        bool yield_new = false;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(intersperse_iterator)

        GENEX_INLINE constexpr intersperse_iterator() = default;

        GENEX_INLINE constexpr intersperse_iterator(I base_it, I first, S last, New new_val) :
            base_it(std::move(base_it)), it(std::move(first)), st(std::move(last)),
            new_value(std::move(new_val)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            if (self.it == self.st) { return self; }
            if (not self.yield_new) { self.yield_new = true; }
            else {
                self.yield_new = false;
                ++self.it;
            }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            if (self.it == self.base_it) { return self; }
            if (self.yield_new) { self.yield_new = false; }
            else {
                self.yield_new = true;
                --self.it;
            }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return self.yield_new ? *&self.new_value : *self.it;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(intersperse_sentinel) {
            if (self.it == self.st) { return true; }

            if (self.yield_new) {
                auto next_it = self.it;
                ++next_it;
                return next_it == self.st;
            }

            return false;
        }
    };


    template <typename I, typename S, typename New>
    requires concepts::interspersable_iters<I, S, New>
    struct intersperse_view {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS New new_value;

        GENEX_INLINE constexpr intersperse_view(I first, S last, New new_val) :
            it(std::move(first)), st(std::move(last)),
            new_value(std::move(new_val)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return intersperse_iterator(self.it, self.it, self.st, self.new_value);
        }

        template <typename Self>
        GENEX_ITER_END {
            return intersperse_sentinel();
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return std::max(0z, iterators::distance(self.it, self.st) * 2 - 1);
        }
    };
}


namespace genex::views {
    struct intersperse_fn {
        template <typename I, typename S, typename New>
        requires detail::concepts::interspersable_iters<I, S, New>
        GENEX_INLINE constexpr auto operator()(I first, S last, New sep) const {
            return detail::impl::intersperse_view(std::move(first), std::move(last), std::move(sep));
        }

        template <typename Rng, typename New>
        requires detail::concepts::interspersable_range<Rng, New>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New sep) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::intersperse_view(std::move(first), std::move(last), std::move(sep));
        }

        template <typename New>
        requires (not range<New>)
        GENEX_INLINE constexpr auto operator()(New sep) const {
            return meta::bind_back(intersperse_fn{}, std::move(sep));
        }
    };

    export inline constexpr intersperse_fn intersperse{};
}
