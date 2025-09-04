#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
BROKEN("Chunk view is broken")


namespace genex::views::concepts {
    template <typename I, typename S>
    concept chunkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept chunkable_range =
        forward_range<Rng> and
        chunkable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::chunkable_iters<I, S>
    auto do_chunk(I first, S last, const std::size_t size) -> generator<generator<iter_value_t<I>>> {
        while (first != last) {
            auto chunk_start = first;
            auto chunk_size = std::min(size, static_cast<std::size_t>(iterators::distance(first, last)));
            auto chunk_end = iterators::next(first, chunk_size);

            auto chunk = [chunk_start, chunk_end]() -> generator<iter_value_t<I>> {
                for (auto it = chunk_start; it != chunk_end; ++it) {
                    co_yield *it;
                }
            }();

            first = chunk_end;
            co_yield std::move(chunk);
        }
    }
}


namespace genex::views {
    struct chunk_fn {
        template <typename I, typename S>
            requires concepts::chunkable_iters<I, S>
        constexpr auto operator()(I first, S last, size_t size) const -> auto {
            return detail::do_chunk(std::move(first), std::move(last), size);
        }

        template <typename Rng>
            requires concepts::chunkable_range<Rng>
        constexpr auto operator()(Rng &&rng, size_t size) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), size);
        }

        constexpr auto operator()(std::size_t n) const -> auto {
            return std::bind_back(chunk_fn{}, n);
        }
    };

    EXPORT_GENEX_STRUCT(chunk);
}
