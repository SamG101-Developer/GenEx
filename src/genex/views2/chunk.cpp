module;
#include <genex/macros.hpp>

export module genex.views2.chunk;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.advance;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename Int>
    concept chunkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::integral<Int>;

    template <typename Rng, typename Int>
    concept chunkable_range =
        forward_range<Rng> and
        chunkable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views2::detail::impl {
    struct chunk_sentinel {};

    template <typename I, typename S, typename Int>
    requires concepts::chunkable_iters<I, S, Int>
    struct chunk_iterator {
        I it; S st;
        Int chunk_size;

        using value_type = std::ranges::subrange<I>;
        using reference_type = std::ranges::subrange<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category =
            std::conditional_t<std::random_access_iterator<I>, std::random_access_iterator_tag,
            std::conditional_t<std::bidirectional_iterator<I>, std::bidirectional_iterator_tag,
            std::forward_iterator_tag>>;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(chunk_iterator);

        GENEX_INLINE constexpr chunk_iterator() = default;

        GENEX_INLINE constexpr chunk_iterator(I first, S last, Int size) :
            it(std::move(first)), st(std::move(last)), chunk_size(size) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            auto n = std::min(self.chunk_size, (Int)iterators::distance(self.it, self.st));
            iterators::advance(self.it, n);
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            auto n = std::min(self.chunk_size, (Int)iterators::distance(self.it, self.st));
            iterators::advance(self.it, -n);
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            auto end_it = self.it;
            for (Int i = 0; i < self.chunk_size and end_it != self.st; ++i) { ++end_it; }
            return std::ranges::subrange(self.it, end_it);
        }

        template <typename Self>
        requires std::random_access_iterator<I>
        GENEX_VIEW_CUSTOM_DEREF {
            auto end_it = self.it + self.chunk_size;
            if (end_it > self.st) { end_it = self.st; }
            return std::ranges::subrange(self.it, end_it);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(chunk_sentinel) {
            return self.it == self.st;
        }
    };

    template <typename I, typename S, typename Int>
    requires concepts::chunkable_iters<I, S, Int>
    struct chunk_view {
        I it; S st;
        Int chunk_size;

        GENEX_INLINE constexpr chunk_view(I first, S last, Int size) :
            it(std::move(first)), st(std::move(last)), chunk_size(size) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return chunk_iterator<I, S, Int>(self.it, self.st, self.chunk_size);
        }

        template <typename Self>
        GENEX_ITER_END {
            return chunk_sentinel();
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return (iterators::distance(self.it, self.st) + self.chunk_size - 1) / self.chunk_size;
        }
    };
}


namespace genex::views2 {
    struct chunk_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::chunkable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int size) const {
            return detail::impl::chunk_view(std::move(first), std::move(last), size);
        }

        template <typename Rng, typename Int>
        requires detail::concepts::chunkable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int size) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::chunk_view(std::move(first), std::move(last), size);
        }

        template <typename Int>
        requires std::integral<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            return meta::bind_back(chunk_fn{}, n);
        }
    };

    export inline constexpr chunk_fn chunk{};
}
