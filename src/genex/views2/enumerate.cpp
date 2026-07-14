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
     * The @c enumerate_sentinel wraps the underlying range's sentinel @c S. It is used as the end of the
     * @c enumerate_view whenever the source range is not common (i.e. its sentinel type differs from its iterator
     * type), because @c enumerate_iterator can only be constructed from an iterator @c I, never from @c S.
     * @tparam S The sentinel type of the underlying iterable.
     */
    template <typename S>
    struct enumerate_sentinel {
        S st;
    };

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
        // `reference`/`pointer` are the legacy nested typedefs std::iterator_traits needs to honour the explicit
        // `iterator_category` below; without `reference` it derives the category from operator* (a prvalue pair) and
        // demotes this to an input iterator, which makes `std::vector(first, last)` skip its reserve-and-copy fast path.
        using reference = value_type;
        using pointer = void;
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

        GENEX_VIEW_ITER_EQ(enumerate_iterator, enumerate_iterator) {
            return self.it == that.it;
        }

        GENEX_VIEW_ITER_EQ(enumerate_iterator, enumerate_sentinel<S>) {
            return self.it == that.st;
        }
    };

    template <typename I, typename S>
    requires concepts::enumerable_iters<I, S>
    struct enumerate_view {
        I it;
        S st;

        GENEX_INLINE constexpr enumerate_view(I it, S st) :
            it(std::move(it)), st(std::move(st)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return enumerate_iterator<I, S>{self.it};
        }

        template <typename Self>
        GENEX_ITER_END {
            if constexpr (std::same_as<I, S>) {
                return enumerate_iterator<I, S>{self.st};
            }
            else {
                return enumerate_sentinel<S>{self.st};
            }
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
        GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
            SAFE_IMPL_CTOR(enumerate_view, I, S) and
            SAFE_MOVE(I) and SAFE_MOVE(S)) {
            return detail::impl::enumerate_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::enumerable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            SAFE_IMPL_CTOR(enumerate_view, iterator_t<Rng>, sentinel_t<Rng>)) {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::enumerate_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const noexcept(
            SAFE_CTOR(enumerate_fn)) {
            return meta::bind_back(enumerate_fn{});
        }
    };

    export inline constexpr enumerate_fn enumerate{};
}
