module;
#include <genex/macros.hpp>

export module genex.views2.cycle;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S>
    concept cyclable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept cyclable_range =
        forward_range<Rng> and
        cyclable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views2::detail::impl {
    struct cycle_sentinel {};

    template <typename I, typename S>
    requires concepts::cyclable_iters<I, S>
    struct cycle_iterator {
        I it; I begin; S st;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category =
            std::conditional_t<std::random_access_iterator<I>, std::random_access_iterator_tag,
            std::conditional_t<std::bidirectional_iterator<I>, std::bidirectional_iterator_tag,
            std::forward_iterator_tag>>;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(cycle_iterator);

        GENEX_INLINE constexpr cycle_iterator() = default;
        GENEX_INLINE constexpr cycle_iterator(I first, I begin, S last) :
            it(std::move(first)), begin(std::move(begin)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            if (self.it == self.st) { self.it = self.begin; }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            if (self.it == self.begin) { self.it = self.st; }
            --self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.it;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(cycle_sentinel) {
            return false;
        }
    };

    template <typename I, typename S>
    requires concepts::cyclable_iters<I, S>
    struct cycle_view {
        I it; I it_cpy; S st;

        GENEX_INLINE constexpr cycle_view(I first, S last) :
            it(std::move(first)), it_cpy(it), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return cycle_iterator<I, S>(self.it, self.it_cpy, self.st);
        }

        template <typename Self>
        GENEX_ITER_END {
            return cycle_sentinel();
        }

        template <typename Self>
        GENEX_ITER_SIZE = delete;
    };
}


namespace genex::views2 {
    struct cycle_fn {
        template <typename I, typename S>
        requires detail::concepts::cyclable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::cycle_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::cyclable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::cycle_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(cycle_fn{});
        }
    };

    export inline constexpr cycle_fn cycle{};
}
