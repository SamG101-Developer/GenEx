#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept chunkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept chunkable_range =
        forward_range<Rng> and
        chunkable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S>
    requires concepts::chunkable_iters<I, S>
    auto do_chunk_n(I first, S last, const std::size_t size) -> generator<std::vector<iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        while (first != last) {
            auto chunk = std::vector<iter_value_t<I>>{};
            for (auto i = 0uz; i < size and first != last; ++i, ++first) {
                chunk.push_back(*first);
            }
            co_yield std::move(chunk);
        }
    }
}


namespace genex::views {
    struct chunk_fn {
        template <typename I, typename S>
        requires detail::concepts::chunkable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last, size_t size) const {
            return detail::coros::do_chunk_n(std::move(first), std::move(last), size);
        }

        template <typename Rng>
        requires detail::concepts::chunkable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, size_t size) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_chunk_n(std::move(first), std::move(last), size);
        }

        GENEX_INLINE constexpr auto operator()(std::size_t n) const {
            return std::bind_back(chunk_fn{}, n);
        }
    };

    inline constexpr chunk_fn chunk{};
}
