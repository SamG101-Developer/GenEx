module;
#include <genex/macros.hpp>

export module genex.views2.move_reverse;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.prev;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S>
    concept move_reversible_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<iter_value_t<I>>;

    template <typename Rng>
    concept move_reversible_range =
        bidirectional_range<Rng> and
        move_reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}



namespace genex::views2::detail::impl {
    template <typename I, typename S>
    requires concepts::move_reversible_iters<I, S>
    struct move_reverse_iterator {
        I it; S st;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(move_reverse_iterator)

        GENEX_INLINE constexpr move_reverse_iterator() = default;

        GENEX_INLINE constexpr move_reverse_iterator(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            --self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            ++self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return std::move(*iterators::prev(self.it));
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(move_reverse_iterator) {
            return self.it == that.it;
        }
    };

    template <typename I, typename S>
    requires concepts::move_reversible_iters<I, S>
    struct move_reverse_view {
        I it; S st;

        GENEX_INLINE constexpr move_reverse_view(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return move_reverse_iterator(self.st, self.it);
        }

        template <typename Self>
        GENEX_ITER_END {
            return move_reverse_iterator(self.it, self.it);
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st);
        }
    };
}


namespace genex::views2 {
    struct move_reverse_fn {
        template <typename I, typename S>
        requires detail::concepts::move_reversible_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::move_reverse_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::move_reversible_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::move_reverse_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(move_reverse_fn{});
        }
    };

    export inline constexpr move_reverse_fn move_reverse{};
}
