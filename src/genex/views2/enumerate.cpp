module;
#include <genex/macros.hpp>

export module genex.views2.enumerate;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept enumerable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept enumerable_range =
        input_range<Rng> and
        enumerable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    /**
     * The @c enumerate_iterator type is an iterator that provides a pair, consisting of the index and the value from
     * the underlying iterable. The index type can be any type that satisfies the @c std::weakly_incrementable concept,
     * such as @c std::size_t or @c int.
     * @tparam I The iterator type of the underlying iterable.
     * @tparam S The sentinel type of the underlying iterable.
     */
    template <typename I, typename S>
    requires concepts::enumerable_iters<I, S>
    struct enumerate_iterator {
        I it;
        std::size_t index = 0uz;

        using value_type = std::pair<std::size_t, iter_value_t<I>>;
        using reference_type = std::pair<std::size_t, iter_reference_t<I>>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(enumerate_iterator);

        GENEX_INLINE constexpr enumerate_iterator() = default;

        GENEX_INLINE explicit constexpr enumerate_iterator(I it) :
            it(std::move(it)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            ++self.index;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            --self.index;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return std::make_pair(self.index, *self.it);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(enumerate_iterator) {
            return self.it == that.it;
        }
    };

    template <typename I, typename S>
    requires concepts::enumerable_iters<I, S>
    struct enumerate_view {
        I it; S st;

        GENEX_INLINE constexpr enumerate_view(I it, S st) :
            it(std::move(it)), st(std::move(st)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return enumerate_iterator<I, S>{self.it};
        }

        template <typename Self>
        GENEX_ITER_END {
            return enumerate_iterator<I, S>{self.st};
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st);
        }
    };
}


namespace genex::views {
    struct enumerate_fn {
        template <typename I, typename S>
        requires detail::concepts::enumerable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::enumerate_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::enumerable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::enumerate_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(enumerate_fn{});
        }
    };

    export inline constexpr enumerate_fn enumerate{};
}
