module;
#include <genex/macros.hpp>

export module genex.views2.reverse;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.iterators.prev;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept reversible_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept reversible_range =
        bidirectional_range<Rng> and
        reversible_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}



namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
    struct reverse_iterator {
        I it;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(reverse_iterator)

        GENEX_INLINE constexpr reverse_iterator() = default;

        GENEX_INLINE constexpr explicit reverse_iterator(I first) :
            it(std::move(first)) {
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
            return *iterators::prev(self.it);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(reverse_iterator) {
            return self.it == that.it;
        }
    };

    template <typename I, typename S>
    requires concepts::reversible_iters<I, S>
    struct reverse_view {
        I it; S st;

        GENEX_INLINE constexpr reverse_view(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return reverse_iterator<I, S>(self.st);
        }

        template <typename Self>
        GENEX_ITER_END {
            return reverse_iterator<I, S>(self.it);
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st);
        }
    };
}


namespace genex::views {
    struct reverse_fn {
        template <typename I, typename S>
        requires detail::concepts::reversible_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::reverse_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::reversible_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::reverse_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(reverse_fn{});
        }
    };

    export inline constexpr reverse_fn reverse{};
}
